<script setup lang="ts">
import { nextTick, onMounted, ref } from 'vue'

let messages = ref([])
let buttonDown = ref(false)
let roomLightsOn = ref(false)
let autoScrollOn = ref(true)
const messageContainer = ref(null)
let ws

onMounted(() => {
  connectWebSocket()
})

const toggleLED = req => {
  ws.send(req)
}

const requestRoomLightState = () => {
  ws.send("REQUEST_ROOM_LIGHT_STATE")
}

const scrollToBottom = () => {
  if(autoScrollOn.value) {
    nextTick(() => {
      if(messageContainer.value) {
        console.log(messageContainer.value.scrollHeight)
        messageContainer.value.scrollTop = messageContainer.value.scrollHeight - 140
      }
    })
  }
}

const connectWebSocket = () => {
  ws = new WebSocket('wss://websocket-5d15bc66efcd.herokuapp.com')
  console.log('Connecting to server')
  ws.onopen = () => {
    console.log('Connected to server')
  }
  ws.onmessage = async event => {
    console.log('Received message from WebSocket:', event.data)
    if (event.data instanceof Blob) {
      const m = await event.data.text()
      console.log(m)
      switch (m) {
        case 'buttonDown':
          buttonDown.value = true
          break
        case 'buttonUp':
          buttonDown.value = false
          break
        case 'roomLightsOn':
          roomLightsOn.value = true;
          break
        case 'roomLightsOff':
          roomLightsOn.value = false;
          break
        default:
      }
      messages.value.push(m)
      scrollToBottom()
    }
  }
  ws.onclose = () => {
    connectWebSocket()
    console.log('WebSocket connection closed')
  }
  ws.onerror = error => {
    console.error('WebSocket error:', error)
  }
}
</script>

<template>
  <div class="pContainer">
    <div class="border">
      <div class="cContainer">
        <h2 style="text-align: center; font-weight: bold">Socket Tools</h2>
      </div>
      <div class="cContainer">
        <button @click="toggleLED('LED_ON')">LED ON</button>
        <button
          @mousedown="toggleLED('LED_ON')"
          @mouseup="toggleLED('LED_OFF')"
          style="border-radius: 100px; width: 50px"
        >
          Hold
        </button>
        <button @click="toggleLED('LED_OFF')">LED OFF</button>
      </div>
      <div class="cContainer" style="align-items: center;">
        <img
          :src="buttonDown ? '/buttonDown.png' : '/buttonUp.png'"
          alt="buttonIcon"
          style="height: 61px; padding: 0 10px;"
        />
        <img
          :src="roomLightsOn ? '/lightBulbOn.png' : '/lightBulbOff.png'"
          alt="lightBulbIpxcon"
          title="Will auto-update when room lights are turned on/off. Click to update now."
          style="height: 80px; padding: 0 10px; cursor: pointer;"
          @click="requestRoomLightState"
        />
      </div>
      <div style="text-align: end; padding-right: 20px; cursor: pointer;" @click="autoScrollOn = !autoScrollOn">Auto Scroll {{autoScrollOn ? 'On' : 'Off' }}</div>
      <div
        class="cContainer"
        ref="messageContainer"
        style="
          flex-direction: column;
          overflow: auto;
          max-height: 100px;
          margin-left: 30px;
          margin-bottom: 30px;
        "
      >
        <div v-for="(message, index) in messages" :key="index">
          {{ index + '. ' + message }}
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
button {
  border: 0;
  border-radius: 6px;
  margin: 0 20px;
  height: 30px;
  width: 80px;
  cursor: pointer;
}

button:hover {
  background-color: #7e7676;
}

.border {
  border: 1px solid white;
  border-radius: 8px;
  width: 500px;
  max-width: 100vw;
}

.pContainer {
  display: flex;
  align-items: center;
  justify-content: center;
  flex-direction: column;
  height: 100vh;
  width: 100%;
}

.cContainer {
  display: flex;
  padding: 50px 0;
  justify-content: center;
}
</style>
