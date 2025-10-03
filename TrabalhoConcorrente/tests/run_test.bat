@echo off
echo Iniciando servidor...
start cmd /k bin\server.exe

timeout /t 1 >nul

echo Iniciando cliente 1...
start cmd /k "echo Cliente1 conectado & bin\client.exe"

echo Iniciando cliente 2...
start cmd /k "echo Cliente2 conectado & bin\client.exe"

echo Iniciando cliente 3...
start cmd /k "echo Cliente3 conectado & bin\client.exe"

echo =======================================
echo Teste de múltiplos clientes iniciado!
echo Digite mensagens nas janelas de clientes.
echo Para sair, use /quit em cada cliente.
echo Logs gravados em server.log e client.log
