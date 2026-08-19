# Run while MudServer.exe is listening on port 7777.
$ErrorActionPreference = 'Stop'

function Receive-Text($client) {
    $stream = $client.GetStream()
    $bytes = New-Object byte[] 4096
    $result = ''
    $timer = [Diagnostics.Stopwatch]::StartNew()
    $lastDataAt = -1
    while ($timer.ElapsedMilliseconds -lt 3000) {
        if ($stream.DataAvailable) {
            $count = $stream.Read($bytes, 0, $bytes.Length)
            if ($count -eq 0) { break }
            $result += [Text.Encoding]::UTF8.GetString($bytes, 0, $count)
            $lastDataAt = $timer.ElapsedMilliseconds
        } elseif ($lastDataAt -ge 0 -and $timer.ElapsedMilliseconds - $lastDataAt -ge 100) {
            break
        } else {
            Start-Sleep -Milliseconds 10
        }
    }
    return $result
}

function Send-Command($client, $command) {
    $bytes = [Text.Encoding]::UTF8.GetBytes($command + "`n")
    $client.GetStream().Write($bytes, 0, $bytes.Length)
}

function Require-Contains($text, $expected) {
    if (-not $text.Contains($expected)) { throw "Missing expected text: $expected`n$text" }
}

$first = [Net.Sockets.TcpClient]::new('127.0.0.1', 7777)
$second = [Net.Sockets.TcpClient]::new('127.0.0.1', 7777)
try {
    $firstStart = Receive-Text $first
    Require-Contains $firstStart '오래된 문'
    Require-Contains $firstStart '출구:'
    if ($firstStart -notmatch '환영합니다, (Player\d+)님!') { throw '플레이어 이름이 없습니다.' }
    $firstName = $Matches[1]
    Require-Contains (Receive-Text $second) '오래된 문'
    Receive-Text $first | Out-Null

    Send-Command $first '말 안녕하세요'
    Require-Contains (Receive-Text $first) "$firstName`님의 말: 안녕하세요"
    Require-Contains (Receive-Text $second) "$firstName`님의 말: 안녕하세요"

    Send-Command $first '북쪽'
    Require-Contains (Receive-Text $first) '무너진 회랑'
    Require-Contains (Receive-Text $second) "$firstName`님이 북쪽으로 떠났습니다"

    Send-Command $first '말 비밀이에요'
    Require-Contains (Receive-Text $first) "$firstName`님의 말: 비밀이에요"
    if ((Receive-Text $second).Contains('비밀이에요')) { throw '다른 방에 대화가 전달되었습니다.' }

    Send-Command $first '북쪽'
    Require-Contains (Receive-Text $first) '그쪽으로는 갈 수 없습니다'

    Send-Command $first '남쪽'
    Require-Contains (Receive-Text $first) '오래된 문'
    Send-Command $first '동쪽'
    Require-Contains (Receive-Text $first) '횃불 방'
    Send-Command $first '서쪽'
    Require-Contains (Receive-Text $first) '오래된 문'

    Send-Command $first '도움말'
    Require-Contains (Receive-Text $first) '명령어:'
    Send-Command $first 'not-a-command'
    Require-Contains (Receive-Text $first) '알 수 없는 명령어'

    Send-Command $first '종료'
    Require-Contains (Receive-Text $first) '안녕히 가세요'

    Send-Command $second '보기'
    Require-Contains (Receive-Text $second) '오래된 문'
    Write-Output 'MUD 한국어 인수 테스트 통과'
} finally {
    $first.Close()
    $second.Close()
}
