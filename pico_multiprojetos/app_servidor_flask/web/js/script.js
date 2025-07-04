// Função para mudar o texto do botão
function mudarTexto(botao) {
    if (botao.innerHTML === "Clique para ligar") {
        botao.innerHTML = "Ligado";
    } else if (botao.innerHTML === "Ligado") {
        botao.innerHTML = "Clique para desligar";
    } else {
        botao.innerHTML = "Clique para ligar";
    }
}
// Função para mostrar uma mensagem de alerta
function mostrarAlerta() {
    alert("Você clicou no botão! 🎉");
}

// controle do ponteiro giratório
const ponteiro = document.getElementById("ponteiro");
const btnEsquerda = document.getElementById("girar-esquerda");
const btnDireita = document.getElementById("girar-direita");

// Centro do joystick (meio dos valores de 0 a 4095)
const centro = 2048;

let anguloAtual = 0;

function atualizarRotacao(novoAngulo) {
    anguloAtual = novoAngulo;
    ponteiro.style.transform = `translateX(-50%) rotate(${anguloAtual}deg)`;
}

// Essa função atualiza o ponteiro com base em x e y
function apontarPara(x, y) {
    const dx = x - centro;
    const dy = centro - y; // Invertido porque o eixo Y cresce para baixo na tela

    // Calcula o ângulo em radianos e depois converte para graus
    const anguloRad = Math.atan2(dy, dx);
    const anguloDeg = anguloRad * (180 / Math.PI);

    // Atualiza a rotação do ponteiro
    ponteiro.style.transform = `translateX(-50%) rotate(${anguloDeg}deg)`;
}

btnEsquerda.addEventListener("click", () => {
    atualizarRotacao(anguloAtual - 15);
});

btnDireita.addEventListener("click", () => {
    atualizarRotacao(anguloAtual + 15);
});


// Teste manual: simula posição x e y de um joystick ou sensor
setTimeout(() => {
    apontarPara(4095, 2048); // Direita
}, 1000);

setTimeout(() => {
    apontarPara(2048, 0); // Cima
}, 2000);

setTimeout(() => {
    apontarPara(0, 2048); // Esquerda
}, 3000);

setTimeout(() => {
    apontarPara(2048, 4095); // Baixo
}, 4000);