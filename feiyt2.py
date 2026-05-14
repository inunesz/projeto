import json
import os


USUARIOS_FILE = "usuarios.json"
VIDEOS_FILE = "videos.json"
FAVORITOS_FILE = "favoritos.json"


def carregar_arquivo(nome_arquivo):
    if not os.path.exists(nome_arquivo):
        with open(nome_arquivo, "w") as f:
            json.dump([], f)

    with open(nome_arquivo, "r") as f:
        return json.load(f)

def salvar_arquivo(nome_arquivo, dados):
    with open(nome_arquivo, "w") as f:
        json.dump(dados, f, indent=4)


def cadastrar_usuario():
    usuarios = carregar_arquivo(USUARIOS_FILE)

    usuario = input("Digite o nome de usuário: ")
    senha = input("Digite a senha: ")

    for u in usuarios:
        if u["usuario"] == usuario:
            print("Usuário já existe!")
            return

    usuarios.append({
        "usuario": usuario,
        "senha": senha
    })

    salvar_arquivo(USUARIOS_FILE, usuarios)
    print("Usuário cadastrado com sucesso!")

def login():
    usuarios = carregar_arquivo(USUARIOS_FILE)

    usuario = input("Usuário: ")
    senha = input("Senha: ")

    for u in usuarios:
        if u["usuario"] == usuario and u["senha"] == senha:
            print("Login realizado com sucesso!")
            return usuario

    print("Usuário ou senha incorretos!")
    return None


def inicializar_videos():
    if not os.path.exists(VIDEOS_FILE):
        videos = [
            {
                "id": 1,
                "nome": "O inicio do curso de Python",
                "likes": 0
            },
            {
                "id": 2,
                "nome": "Jogo da seleção brasileira de 2002",
                "likes": 0
            },
            {
                "id": 3,
                "nome": "Historia dos povos primitivos",
                "likes": 0
            }
        ]

        salvar_arquivo(VIDEOS_FILE, videos)

def listar_videos():
    videos = carregar_arquivo(VIDEOS_FILE)

    print("\n===== VÍDEOS =====")

    for video in videos:
        print(f"""
ID: {video['id']}
Nome: {video['nome']}
Likes: {video['likes']}
""")

def buscar_video():
    videos = carregar_arquivo(VIDEOS_FILE)

    nome = input("Digite o nome do vídeo: ").lower()

    encontrados = []

    for video in videos:
        if nome in video["nome"].lower():
            encontrados.append(video)

    if encontrados:
        print("\nVídeos encontrados:\n")

        for video in encontrados:
            print(f"""
ID: {video['id']}
Nome: {video['nome']}
Likes: {video['likes']}
""")
    else:
        print("Nenhum vídeo encontrado!")

def curtir_video():
    videos = carregar_arquivo(VIDEOS_FILE)

    listar_videos()

    video_id = int(input("Digite o ID do vídeo: "))

    for video in videos:
        if video["id"] == video_id:
            video["likes"] += 1
            salvar_arquivo(VIDEOS_FILE, videos)
            print("Vídeo curtido!")
            return

    print("Vídeo não encontrado!")

def descurtir_video():
    videos = carregar_arquivo(VIDEOS_FILE)

    listar_videos()

    video_id = int(input("Digite o ID do vídeo: "))

    for video in videos:
        if video["id"] == video_id:
            if video["likes"] > 0:
                video["likes"] -= 1

            salvar_arquivo(VIDEOS_FILE, videos)
            print("Like removido!")
            return

    print("Vídeo não encontrado!")


def criar_playlist(usuario):
    favoritos = carregar_arquivo(FAVORITOS_FILE)

    nome_playlist = input("Nome da playlist: ")

    favoritos.append({
        "usuario": usuario,
        "playlist": nome_playlist,
        "videos": []
    })

    salvar_arquivo(FAVORITOS_FILE, favoritos)

    print("Playlist criada!")

def listar_playlists(usuario):
    favoritos = carregar_arquivo(FAVORITOS_FILE)

    print("\n===== PLAYLISTS =====")

    for fav in favoritos:
        if fav["usuario"] == usuario:
            print(f"""
Playlist: {fav['playlist']}
Vídeos: {fav['videos']}
""")

def editar_playlist(usuario):
    favoritos = carregar_arquivo(FAVORITOS_FILE)

    nome = input("Digite o nome da playlist: ")

    for fav in favoritos:
        if fav["usuario"] == usuario and fav["playlist"] == nome:
            novo_nome = input("Novo nome: ")
            fav["playlist"] = novo_nome

            salvar_arquivo(FAVORITOS_FILE, favoritos)

            print("Playlist editada!")
            return

    print("Playlist não encontrada!")

def excluir_playlist(usuario):
    favoritos = carregar_arquivo(FAVORITOS_FILE)

    nome = input("Digite o nome da playlist: ")

    favoritos = [
        fav for fav in favoritos
        if not (fav["usuario"] == usuario and fav["playlist"] == nome)
    ]

    salvar_arquivo(FAVORITOS_FILE, favoritos)

    print("Playlist excluída!")

def adicionar_video_playlist(usuario):
    favoritos = carregar_arquivo(FAVORITOS_FILE)
    videos = carregar_arquivo(VIDEOS_FILE)

    nome_playlist = input("Nome da playlist: ")

    listar_videos()

    video_id = int(input("ID do vídeo: "))

    video_encontrado = None

    for video in videos:
        if video["id"] == video_id:
            video_encontrado = video["nome"]

    if not video_encontrado:
        print("Vídeo não encontrado!")
        return

    for fav in favoritos:
        if fav["usuario"] == usuario and fav["playlist"] == nome_playlist:
            fav["videos"].append(video_encontrado)

            salvar_arquivo(FAVORITOS_FILE, favoritos)

            print("Vídeo adicionado!")
            return

    print("Playlist não encontrada!")

def remover_video_playlist(usuario):
    favoritos = carregar_arquivo(FAVORITOS_FILE)

    nome_playlist = input("Nome da playlist: ")

    for fav in favoritos:
        if fav["usuario"] == usuario and fav["playlist"] == nome_playlist:

            print(f"Vídeos: {fav['videos']}")

            video_nome = input("Nome do vídeo para remover: ")

            if video_nome in fav["videos"]:
                fav["videos"].remove(video_nome)

                salvar_arquivo(FAVORITOS_FILE, favoritos)

                print("Vídeo removido!")
                return

    print("Playlist ou vídeo não encontrado!")


def menu_usuario(usuario):

    while True:

        print(f"""
======== MENU ========

Usuário: {usuario}

1 - Listar vídeos
2 - Buscar vídeo
3 - Curtir vídeo
4 - Descurtir vídeo
5 - Criar playlist
6 - Listar playlists
7 - Editar playlist
8 - Excluir playlist
9 - Adicionar vídeo na playlist
10 - Remover vídeo da playlist
0 - Sair
""")

        opcao = input("Escolha: ")

        if opcao == "1":
            listar_videos()

        elif opcao == "2":
            buscar_video()

        elif opcao == "3":
            curtir_video()

        elif opcao == "4":
            descurtir_video()

        elif opcao == "5":
            criar_playlist(usuario)

        elif opcao == "6":
            listar_playlists(usuario)

        elif opcao == "7":
            editar_playlist(usuario)

        elif opcao == "8":
            excluir_playlist(usuario)

        elif opcao == "9":
            adicionar_video_playlist(usuario)

        elif opcao == "10":
            remover_video_playlist(usuario)

        elif opcao == "0":
            break

        else:
            print("Opção inválida!")


def main():

    inicializar_videos()

    while True:

        print("""
======= SISTEMA =======

1 - Cadastrar
2 - Login
0 - Sair
""")

        opcao = input("Escolha: ")

        if opcao == "1":
            cadastrar_usuario()

        elif opcao == "2":
            usuario = login()

            if usuario:
                menu_usuario(usuario)

        elif opcao == "0":
            break

        else:
            print("Opção inválida!")

main()