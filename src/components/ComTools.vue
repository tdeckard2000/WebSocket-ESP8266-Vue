<script setup lang="ts">
import { onMounted, ref } from 'vue'

let messages = ref([])
let buttonDown = ref(false)

onMounted(() => {
  connectWebSocket()
})

const connectWebSocket = () => {
  const ws = new WebSocket('wss://websocket-5d15bc66efcd.herokuapp.com')
  console.log('Connecting to server')
  ws.onopen = () => {
    console.log('Connected to server')
  }
  ws.onmessage = async event => {
    console.log('Received message from WebSocket:', event.data)
    if (event.data instanceof Blob) {
      const m = await event.data.text()
      console.log(m)
      if (m === 'Button Down') {
        buttonDown.value = true
      } else if (m === 'Button Up') {
        buttonDown.value = false
      }
      messages.value.push(m)
    }
  }
  ws.onclose = () => {
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
        <button>LED ON</button>
        <button>LED OFF</button>
      </div>
      <div class="cContainer">
        <img
          :src="buttonDown ? '/buttonDown.png' : '/buttonUp.png'"
          alt="buttonIcon"
        />
      </div>
      <div
        class="cContainer"
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
