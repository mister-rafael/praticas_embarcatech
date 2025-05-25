const socket = io();

const statusBox = document.getElementById('status-box');
const botaoA = document.getElementById('botaoA');
const botaoB = document.getElementById('botaoB');

const xSpan = document.getElementById('x-pos');
const ySpan = document.getElementById('y-pos');

// Controla os estados individuais
let estadoA = 'solto';
let estadoB = 'solto';

socket.on('connect', () => {
  console.log('Conectado ao servidor via Socket.IO');
});

socket.on('command', (data) => {
  if (data.action === 'click_a') {
    if (estadoA != 'pressionado') { // Verifica se o botão A não está pressionado
      botaoA.classList.add('pressionado');
      statusBox.textContent = 'Botão A, pressionado!';
      estadoA = 'pressionado';

    }
  } else if (data.action === 'solto_a') {
    if (estadoA == 'pressionado') { // Verifica se o botão A está pressionado
      botaoA.classList.remove('pressionado');
      statusBox.textContent = 'Botão A, solto!';
      estadoA = 'solto';
    }
  }
  //VERIFICAR BOTÃO B
  if (data.action === 'click_b') {
    if (estadoB != 'pressionado') { // Verifica se o botão B não está pressionado
      botaoB.classList.add('pressionado');
      statusBox.textContent = 'Botão B, pressionado!';
      estadoB = 'pressionado';
    }
  } else if (data.action === 'solto_b') {
    if (estadoB == 'pressionado') { // Verifica se o botão B está pressionado
      botaoB.classList.remove('pressionado');
      statusBox.textContent = 'Botão B, solto!';
      estadoB = 'solto';
    }
  }

  // Atualiza mensagem central

  //statusBox.textContent = `Botão A: ${estadoA.toUpperCase()} | Botão B: ${estadoB.toUpperCase()}`;

  if (estadoA === 'pressionado' || estadoB === 'pressionado') {
    statusBox.style.backgroundColor = 'green';
    statusBox.innerHTML = `Botão A: ${estadoA.toUpperCase()}<br>Botão B: ${estadoB.toUpperCase()}`;
  } else {
    statusBox.style.backgroundColor = 'red';
    statusBox.textContent = `Aguardando...`;
  }
});
// Listener para receber os valores do joystick
socket.on('joystick', (data) => {
  // Atualiza os spans dos valores X e Y
  xSpan.textContent = data.x;
  ySpan.textContent = data.y;
  console.log(`Joystick: X = ${data.x}, Y = ${data.y}`);
});