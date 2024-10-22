import express from 'express'
import path from 'path'
import { dirname } from 'path'
import { fileURLToPath } from 'url'
import { WebSocketServer } from 'ws'
import { createServer } from 'http'

const app = express()
const __filename = fileURLToPath(import.meta.url)
const __dirname = dirname(__filename)
const server = createServer(app) // create http server
const wss = new WebSocketServer({ server }) // create websocket server using http server
app.use(express.static(path.join(__dirname, 'dist')))

wss.on('connection', ws => {
  console.log('WebSocket connection established')
  ws.on('message', message => {
    console.log(`Received message => ${message}`)
    ws.send(`Received: ${message}`)
    wss.clients.forEach(client => {
      client.send(message)
    })
  })
  ws.on('error', err => {
    console.error(err)
  })
  ws.send('Connected to WebSocket server')
})

app.get('/api', (req, res) => {
  const { send } = req.query;
  console.log('Received text: ', send);
  wss.clients.forEach(client => {
    console.log("client", client.readyState)
    if(client.readyState === client.OPEN) {
      console.log("send: ", send)
      client.send(send);
    }
  })
  res.send(`Text logged successfully: ${send}`);
});

app.get('*', (req, res) => {
  console.log('req: ', req.url)
  console.log('dirname: ', __dirname)
  res.sendFile(path.join(__dirname, 'dist', 'index.html'))
})

const PORT = process.env.PORT || 3000
server.listen(PORT, () => {
  console.log('Process.env.PORT: ', process.env.PORT)
  console.log(`Server started on port ${PORT}`)
})
