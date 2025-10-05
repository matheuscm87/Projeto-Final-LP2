#!/bin/bash

# Garante que os binários existam
make all

echo "Iniciando servidor..."
./bin/server &

# Dá um tempo pro servidor subir
sleep 1

echo "Iniciando cliente 1..."
(echo "cli1"; sleep 0.1; echo "Mensagem do cliente 1"; sleep 1; echo "/quit") | ./bin/client &

echo "Iniciando cliente 2..."
(echo "cli2"; sleep 0.1; echo "Mensagem do cliente 2"; sleep 1; echo "/quit") | ./bin/client &

echo "Iniciando cliente 3..."
(echo "cli3"; sleep 0.1; echo "Mensagem do cliente 3"; sleep 1; echo "/quit") | ./bin/client &

wait
echo "Teste concluído. Veja logs em logs/"
