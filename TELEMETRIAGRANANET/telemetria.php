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
        .container {
            display: flex;
            flex-direction: column; /* Alterado para coluna */
        }
        .card {
            border: 1px solid #ddd;
            margin-bottom: 20px;
            padding: 10px;
        }
        table {
            border-collapse: collapse;
            width: 100%;
        }
        th, td {
            border: 1px solid #ddd;
            padding: 8px;
            text-align: left;
        }
        th {
            background-color: #f2f2f2;
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

    <div class="container">
        <?php
        try {
            $pdo = new PDO('mysql:host=santocyber.helioho.st;dbname=santocyber_telemetria', 'santocyber_telemetria', 'telemetria');
            $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

            if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_POST['mac'], $_POST['action'])) {
                $mac = $_POST['mac'];
                $action = $_POST['action'];

                // Atualize a coluna 'acoes' na tabela 'telemetria' com a ação selecionada
                $stmt = $pdo->prepare("UPDATE telemetria SET acoes = :acoes WHERE mac = :mac");
                $stmt->bindParam(':acoes', $action, PDO::PARAM_STR);
                $stmt->bindParam(':mac', $mac, PDO::PARAM_STR);
                $stmt->execute();
            }

            $consulta = $pdo->query("SELECT identificador, nome, ip, granaentrada, objsaida, mac, timelocal, wifilocal, last_update, acoes FROM telemetria;");
            $somaGranaEntrada = 0; // Variável para armazenar a soma dos valores de 'granaentrada'

            while ($linha = $consulta->fetch(PDO::FETCH_ASSOC)) {
                echo "<div class='card'>";
                echo "<table>";
                echo "<tr><th>Informações</th><th>Ação</th></tr>";
                echo "<tr><td>Registro N: {$linha['identificador']}</td><td rowspan='2'><form method='POST'><input type='hidden' name='mac' value='{$linha['mac']}'><select name='action'><option value='ligado'>Ligar</option><option value='desligado'>Desligar</option><option value='reinicia'>Reiniciar</option><option value='credito'>Crédito</option></select><button type='submit'>Executar Ação</button></form></td></tr>";
                echo "<tr><td>Nome: {$linha['nome']}</td></tr>";
                echo "<tr><td>Valor entrada: {$linha['granaentrada']}</td><td>Ação: {$linha['acoes']}</td></tr>";
                echo "<tr><td>Quantidade Saida: {$linha['objsaida']}</td></tr>";
                echo "<tr><td>Endereço MAC: {$linha['mac']}</td></tr>";
                echo "<tr><td>IP local: {$linha['ip']}</td></tr>";
                echo "<tr><td>WiFilocal: {$linha['wifilocal']}</td></tr>";
                echo "<tr><td>Hora no local: {$linha['timelocal']}</td></tr>";
                echo "<tr><td>Última Atualização: {$linha['last_update']}</td></tr>";
                echo "</table>";
                echo "</div>";

                // Adicione o valor de 'granaentrada' à soma
                $somaGranaEntrada += $linha['granaentrada'];
            }

            // Exiba a soma dos valores de 'granaentrada' no final da página
            echo "<div class='card'>";
            echo "<table>";
            echo "<tr><th>Total de granaentrada</th></tr>";
            echo "<tr><td>{$somaGranaEntrada}</td></tr>";
            echo "</table>";
            echo "</div>";
        } catch (PDOException $e) {
            echo "Erro: " . $e->getMessage();
        }
        ?>
    </div>
</body>
</html>
