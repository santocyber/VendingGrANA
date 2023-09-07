<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Telemetria GrANANET</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 10px;
            padding: 0;
        }
        h1 {
            text-align: center;
        }
        select {
            width: 100%;
            padding: 5px;
            margin-bottom: 10px;
        }
        button {
            width: 100%;
            padding: 10px;
            background-color: #007BFF;
            color: #fff;
            border: none;
            cursor: pointer;
        }
    </style>
</head>
<body>
    <h1>Telemetria GrANANET</h1>

    <form method="POST" action="executa_action.php"> <!-- Substitua "executa_action.php" pelo nome do seu script PHP de execução de ações -->
        <?php
        try {
            $pdo = new PDO('mysql:host=santocyber.helioho.st;dbname=santocyber_telemetria', 'santocyber_telemetria', 'telemetria');
            $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

            $consulta = $pdo->query("SELECT identificador, nome, ip, granaentrada, objsaida, mac, timelocal, wifilocal, last_update, acoes FROM telemetria;");

            while ($linha = $consulta->fetch(PDO::FETCH_ASSOC)) {
                echo "<p>Registro N: {$linha['identificador']} - Nome: {$linha['nome']} - Valor entrada: {$linha['granaentrada']} - Quantidade Saida: {$linha['objsaida']} - Endereço MAC: {$linha['mac']} - IP local: {$linha['ip']} - WiFilocal: {$linha['wifilocal']} - Ação: {$linha['acoes']} - Hora local: {$linha['timelocal']} - Última Atualização: {$linha['last_update']}</p>";
                echo "<select name='action[{$linha['mac']}]'>";
                echo "<option value='ligado'>Ligar</option>";
                echo "<option value='desligado'>Desligar</option>";
                echo "<option value='reinicia'>Reiniciar</option>";
                echo "<option value='credito'>Crédito</option>";
                echo "</select>";
            }
        } catch (PDOException $e) {
            echo "Erro: " . $e->getMessage();
        }
        ?>
        <button type="submit">Executar Ação</button>
    </form>
</body>
</html>
