import json
import os

ARQUIVO = "dados.json"

if not os.path.exists(ARQUIVO):
    dados_iniciais = {
        "usuarios": [],
        "videos": [
            {"id": 1, "nome": "Curso Python", "curtidas": 0, "descurtidas": 0},
            {"id": 2, "nome": "Futebol brasileiro", "curtidas": 0, "descurtidas": 0}
        ],
        "favoritos": []
    }

    with open(ARQUIVO, "w", encoding="utf-8") as arq:
        json.dump(dados_iniciais, arq, indent=4)


def carregar():
    with open(ARQUIVO, "r", encoding="utf-8") as arq:
        return json.load(arq)


def salvar(dados):
    with open(ARQUIVO, "w", encoding="utf-8") as arq:
        json.dump(dados, arq, indent=4)


def cadastrar():
    dados = carregar()

    nome = input("Nome: ")
    senha = input("Senha: ")

    usuario = {
        "nome": nome,
        "senha": senha
    }

    dados["usuarios"].append(usuario)
    salvar(dados)

    print("Usuário cadastrado!")


def login():
    dados = carregar()

    nome = input("Nome: ")
    senha = input("Senha: ")

    for usuario in dados["usuarios"]:
        if usuario["nome"] == nome and usuario["senha"] == senha:
            print("Login realizado!")
            return nome

    print("Usuário não encontrado.")
    return None



def buscar_video():
    dados = carregar()

    busca = input("Digite o nome do vídeo: ")

    for video in dados["videos"]:
        if busca.lower() in video["nome"].lower():
            print(video)


def curtir():
    dados = carregar()

    video_id = int(input("ID do vídeo: "))

    for video in dados["videos"]:
        if video["id"] == video_id:
            video["curtidas"] += 1

    salvar(dados)
    print("Vídeo curtido!")


def descurtir():
    dados = carregar()

    video_id = int(input("ID do vídeo: "))

    for video in dados["videos"]:
        if video["id"] == video_id:
            video["descurtidas"] += 1

    salvar(dados)
    print("Vídeo descurtido!")


def adicionar_favorito(usuario):
    dados = carregar()

    video_id = int(input("ID do vídeo: "))

    favorito = {
        "usuario": usuario,
        "video_id": video_id
    }

    dados["favoritos"].append(favorito)
    salvar(dados)

    print("Adicionado aos favoritos!")



usuario_logado = None

while True:
    print("\n1-Cadastrar")
    print("2-Login")
    print("3-Buscar vídeo")
    print("4-Curtir vídeo")
    print("5-Descurtir vídeo")
    print("6-Adicionar favorito")
    print("0-Sair")

    op = input("Escolha: ")

    if op == "1":
        cadastrar()

    elif op == "2":
        usuario_logado = login()

    elif op == "3":
        buscar_video()

    elif op == "4":
        curtir()

    elif op == "5":
        descurtir()

    elif op == "6":
        if usuario_logado:
            adicionar_favorito(usuario_logado)
        else:
            print("Faça login primeiro.")

    elif op == "0":
        break