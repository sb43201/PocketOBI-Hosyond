// Browser/API contract tests for the embedded OBI-style dashboard.
const test = require('node:test');
const assert = require('node:assert/strict');
const vm = require('node:vm');
const fs = require('node:fs');
const script = fs.readFileSync('web_interface.h', 'utf8').match(/<script>([\s\S]*?)<\/script>/)[1];
function fixture() {
  const nodes = new Map(), requests = [], buttons = [{disabled: false}, {disabled: false}];
  let response = {connected: true, ssid: 'Test WiFi', ip: '192.168.1.7', rssi: -60, bridge: false};
  let ok = true;
  const get = id => {
    if (!nodes.has(id)) nodes.set(id, {textContent: '', className: '', scrollHeight: 0, scrollTop: 0,
      classList: {remove() {}, add() {}}});
    return nodes.get(id);
  };
  const context = vm.createContext({
    document: {getElementById: get, querySelectorAll: () => buttons},
    Date, AbortController, setTimeout, clearTimeout,
    confirm: () => true,
    fetch: async (url, options) => {
      requests.push({url, options});
      return {ok, status: ok ? 200 : 409, json: async () => response};
    }
  });
  vm.runInContext(script, context);
  return {context, requests, buttons, get, setReply(value, success = true) {response = value; ok = success;}};
}
const settle = () => new Promise(resolve => setImmediate(resolve));
const pack = () => ({success: true, model: 'BL1850B', locked: false, verdict: 'HEALTHY',
  chargeCount: 0, mfgDate: '2021-06-14', capacity: 5, errorCode: 69, packVoltage: 20,
  cell1: 4, cell2: 4, cell3: 4, cell4: 4, cell5: 4, cellDiff: 0,
  tempCell: 0, tempMosfet: 0});

test('connection checks are GET; battery reads are POST with the device header', async () => {
  const f = fixture(); await settle();
  assert.equal(f.requests[0].url, '/api/status');
  assert.equal(f.requests[0].options.method, 'GET');
  f.setReply(pack()); await f.context.readBattery();
  const req = f.requests.at(-1);
  assert.equal(req.url, '/api/read'); assert.equal(req.options.method, 'POST');
  assert.equal(req.options.headers['X-PocketOBI'], '1');
  assert.equal(f.get('chargeCount').textContent, 0);
  assert.match(f.get('tempMosfet').textContent, /^0\.0/);
  assert.equal(f.get('status').textContent, 'HEALTHY');
  assert.ok(f.buttons.every(button => !button.disabled));
});

test('bridge rejection reports the reason and clears old readings', async () => {
  const f = fixture(); await settle();
  f.setReply(pack()); await f.context.readBattery();
  f.setReply({success: false, error: 'USB PC bridge is selected.'}, false);
  await f.context.readBattery();
  assert.equal(f.get('model').textContent, '--');
  assert.match(f.get('debugLog').textContent, /USB PC bridge is selected/);
  assert.ok(f.buttons.every(button => !button.disabled));
});

test('LED writes use POST form data and report that the command was sent', async () => {
  const f = fixture(); await settle();
  f.setReply({success: true, message: 'LED command sent.'});
  await f.context.testLeds(true);
  assert.equal(f.requests.at(-1).url, '/api/leds');
  assert.equal(f.requests.at(-1).options.method, 'POST');
  assert.equal(f.requests.at(-1).options.body, 'state=1');
  assert.match(f.get('debugLog').textContent, /LED command sent/);
});

test('reset cancellation sends nothing, confirmed reset sends an explicit confirmation', async () => {
  const f = fixture(); await settle();
  f.context.confirm = () => false;
  const count = f.requests.length; await f.context.resetErrors();
  assert.equal(f.requests.length, count);
  f.context.confirm = () => true;
  f.setReply({success: false, message: 'Reset sent, verification failed: outcome unknown.'});
  await f.context.resetErrors();
  assert.equal(f.requests.at(-1).url, '/api/reset');
  assert.equal(f.requests.at(-1).options.body, 'confirm=reset');
  assert.match(f.get('debugLog').textContent, /outcome unknown/);
});

test('voltage refresh also replaces identity when the connected pack changes', async () => {
  const f = fixture(); await settle();
  f.setReply(pack()); await f.context.readBattery();
  f.setReply({...pack(), model: 'BL1820B', capacity: 2});
  await f.context.readVoltages();
  assert.equal(f.get('model').textContent, 'BL1820B');
  assert.equal(f.get('capacity').textContent, '2 Ah');
});
