# SweetHTTP
# Um servidor HTTP implementado usando SweetSocket 🌐

---

## Sobre ℹ️

SweetHTTP é um servidor HTTP/1.1 compatível, desenvolvido com base no paralelismo em threads da SweetSocket.

Ele tem as seguintes funcionalidades:

- 📄 Documento padrão
- 📂 Listagem de diretórios
- 🌐 Path em UTF-16 e URL Encoded
- ⚙️ Configuração via arquivo
- 📝 Log de acesso
- 🧩 Interface de plugins dinâmicos
- ⬇️ Download parcial
- 🔗 Conexão em Keep-Alive
- 🌍 Multiplas portas em multiplos endereços
- 🐧 Em breve com suporte a Linux

## Requisitos 🖥️

- Windows NT 5 e superior

## Futuro 🚀

Como futuro, pretende-se colocar suporte a interface fast-cgi e cgi-bin, além do suporte aos servidores virtuais.

## Plugins 🧩

O servidor tem um sistema de plugins carregados em bibliotecas dinâmicas. No caso do Windows, pode-se carregar uma DLL que será usada como processador da requisição em vez de acessar um arquivo.

Existem dois tipos de entradas planejadas para serem aceitas pelos plugins:

- 📥 Requests

    Nesse ponto, o plugin decide se pode ou não acessar, pode reescrever a URL, redirecionar ou encerrar o socket. Resumidamente, tem controle do header do HTTP. (Não implementado)

- 📤 Response

    Aqui, o plugin captura o header e com isso decide o que e como retornar. Pode ser usado para criar APIs e sistemas dinâmicos baseados na WEB 2. (Parcialmente implementado)

## Stack de tecnologia 💻

Esse servidor pretende ser o API mínimo dependente, tentará extrair o máximo de libs já fornecidas no SO.

- No Windows

    - Winsock2
    - WINAPI

- No Linux

    - pThreads

## Como buildar 🛠️

No Windows, pode-se usar o Visual Studio usando os SLN inclusos ou o MSYS2 como ambiente mingw.

- Primeiro, deve-se clonar esse repositório e seu subrepositório

```sh
git clone --recurse-submodules https://github.com/Ruaneri-Portela/SweetHTTP.git
cd SweetHTTP
```

- No ambiente MSYS2, basta ter a toolchain correta para C instalada e rodar o Make para compilar e rodar...

```sh
make
.\build\SweetHTTP
```

- Para o Visual Studio, compile usando o SLN incluso.
