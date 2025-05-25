# Importa as bibliotecas Flask e SocketIO
from flask import Flask, render_template, request 
from flask_socketio import SocketIO, emit, send

# Cria a instância do Flask
app = Flask(__name__, template_folder='templates_html')

# Configura o SocketIO para permitir conexões de qualquer origem
socketio = SocketIO(app, cors_allowed_origins="*")

# Rota principal que serve a página HTML
@app.route('/')
def index():
    return render_template('index.html')  # Renderiza o template web/templates/index.html
# Rota para servir outras páginas HTML
#@app.route('/pratica_botoes')
#def serve_page(pratica_botoes):
    # Renderiza o template correspondente ao nome da página
 #   return render_template(pratica_botoes)
# ==============================================================================================

@app.route('/CLICK_A', methods=['GET', 'POST']) # Define a rota para o comando de clique
# Define uma função para lidar com o evento de clique do botão A
def click_a():
    print("Comando: Botão A, pressionado")
    socketio.emit('command', {'action': 'click_a'})  # Envia comando para ON
    return 'Click command sent', 200 # Retorna resposta HTTP 200

@app.route('/SOLTO_A', methods=['GET', 'POST']) # Define a rota para o comando de solto
def solto_a():
    print("Comando: Botão A, solto")
    socketio.emit('command', {'action': 'solto_a'})  # Envia comando para OFF
    return 'solto command sent', 200

# Define uma função para lidar com o evento de clique do botão B

@app.route('/CLICK_B', methods=['GET', 'POST']) # Define a rota para o comando de clique
# Define uma função para lidar com o evento de clique do botão A
def click_b():
    print("Comando: Botão B, pressionado")
    socketio.emit('command', {'action': 'click_b'})  # Envia comando para ON
    return 'Click command sent', 200 # Retorna resposta HTTP 200

@app.route('/SOLTO_B', methods=['GET', 'POST']) # Define a rota para o comando de solto
def solto_b():
    print("Comando: Botão B, solto")
    socketio.emit('command', {'action': 'solto_b'})  # Envia comando para OFF
    return 'solto command sent', 200

@app.route('/joystick', methods=['GET'])
def joystick():
    x = request.args.get('x', type=int)
    y = request.args.get('y', type=int)
    print(f"Dados do joystick recebidos: X = {x}, Y = {y}")
    socketio.emit('joystick', {'x': x, 'y': y})  # Envia os dados do joystick para o cliente
    return "Dados recebidos", 200


#=============================================================================================
# Ponto de entrada principal da aplicação
if __name__ == '__main__':
    # Inicia o servidor Flask com suporte a WebSockets
    socketio.run(app, host='0.0.0.0', port=5000) # Permite conexões de qualquer IP na porta 5000
