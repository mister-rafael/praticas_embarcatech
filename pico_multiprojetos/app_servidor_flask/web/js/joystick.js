// script_rosa.js
// Este arquivo contém o código JavaScript para a página da bússola
const socket = io(); // Conexão com o servidor Flask-SocketIO
const xSpan = document.getElementById('x-pos');
const ySpan = document.getElementById('y-pos');

socket.on('connect', () => {
    console.log('Conectado ao servidor via Socket.IO');
});

document.addEventListener('DOMContentLoaded', function () {
    const compassStar = document.getElementById('compassStar');
    const angleDisplay = document.getElementById('angleDisplay');
    const rotateLeftBtn = document.getElementById('rotateLeft');
    const rotateRightBtn = document.getElementById('rotateRight');
    const resetPointerBtn = document.getElementById('resetPointer');
    const degreeMarks = document.getElementById('degreeMarks');

    let currentAngle = 0;

    // Criar marcas de graus
    for (let i = 0; i < 360; i += 5) {
        const mark = document.createElement('div');
        mark.className = i % 30 === 0 ? 'degree-mark major' : 'degree-mark';
        mark.style.transform = `rotate(${i}deg) translateX(-50%) translateY(-100%)`;
        degreeMarks.appendChild(mark);
    }

    // Função para atualizar o ponteiro
    function updatePointer(angle) {
        currentAngle = angle;

        // Mantém o ângulo entre 0 e 359
        if (currentAngle < 0) currentAngle += 360;
        if (currentAngle >= 360) currentAngle %= 360;

        // Atualiza a rotação da estrela com animação suave
        compassStar.style.transform = `translate(-50%, -50%) rotate(${currentAngle}deg)`;

        // Atualiza o display do ângulo
        angleDisplay.textContent = `${Math.round(currentAngle)}°`;
    }

    // Função para controlar o ponteiro via joystick
    function controlPointerWithJoystick(x, y) {
        // Converte coordenadas x,y do joystick para um ângulo
        // Assumindo que x e y estão normalizados entre -1 e 1
        if (Math.abs(x) < 0.1 && Math.abs(y) < 0.1) {
            // Zona morta para evitar flutuações quando o joystick está em repouso
            return;
        }

        let angle = Math.atan2(y, x) * (180 / Math.PI);

        // Ajusta para que 0 graus seja Norte (para cima)
        angle = 90 - angle;
        if (angle < 0) angle += 360;


        updatePointer(angle);
    }

    // Botões de controle para teste
    rotateLeftBtn.addEventListener('click', function () {
        updatePointer(currentAngle - 15);
    });

    rotateRightBtn.addEventListener('click', function () {
        updatePointer(currentAngle + 15);
    });

    resetPointerBtn.addEventListener('click', function () {
        updatePointer(0);
    });
    // Crie essas funções apenas uma vez:
    window.setCompassAngle = function (angle) {
        updatePointer(angle);
    };

    window.setJoystickPosition = function (x, y) {
        controlPointerWithJoystick(x, y); // agora usa os argumentos corretamente
    };
    // Listener para receber os valores do joystick
    socket.on('joystick', (data) => {
        // Atualiza os spans dos valores X e Y
        xSpan.textContent = data.x;
        ySpan.textContent = data.y;
        console.log(`Joystick: X = ${data.x}, Y = ${data.y}`);
        
        // Atualiza o ponteiro da bússola com base nos valores do joystick
        controlPointerWithJoystick(data.x, data.y);

    });

    // Inicializa o ponteiro
    updatePointer(0);


    // Adiciona controle por teclado para testes
    document.addEventListener('keydown', function (e) {
        switch (e.key) {
            case 'ArrowLeft':
                updatePointer(currentAngle - 5);
                break;
            case 'ArrowRight':
                updatePointer(currentAngle + 5);
                break;
            case ' ':
                updatePointer(0);
                break;
        }
    });
});