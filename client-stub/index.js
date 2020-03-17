const net = require('net');

const host = '192.168.1.105';
const port = 3333;

const client = net.createConnection(port, host, () => {
  console.log(`connected to ${host}:${port}`);
});

client.setEncoding('ascii');

client.on('data', (data) => {
  console.log(`data received: ${data}`);
  client.destroy();
})

client.on('close', () => {
  console.log(`connection closed`);
});

client.write('R1\r\n', 'ascii', () => {
  console.log('data written');
})