# Caminho absoluto para o script getVersion.ps1
$scriptPath = Join-Path -Path (Get-Location) -ChildPath "utils\genVersion.ps1"

# Verifique se o script existe
if (Test-Path $scriptPath) {
    . $scriptPath
} else {
    Write-Output "O script getVersion.ps1 não foi encontrado em: $scriptPath"
}

$projectName = "SweetHTTP"
$builds = @("x64")
$configuration = @("Release", "Debug")
$buildDir = ".\build"
$version = "$GIT_TAG-$GIT_HASH"

# Limpar e compilar usando make
make clean
make

# Empacotar para MSVC
foreach ($build in $builds) {
    $msbuildPath = "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"

    if (-not (Test-Path $msbuildPath)) {
        Write-Host "MSBuild não encontrado em $msbuildPath"
    } else {
        Write-Host "MSBuild encontrado em $msbuildPath"

        foreach ($conf in $configuration) {
            # Executar MSBuild para a configuração e plataforma
            & $msbuildPath "$projectName.sln" -p:Configuration=$conf -p:Platform=$build

            # Criar diretório para o build
            $buildPath = "$buildDir\$build\$conf"
            New-Item -ItemType Directory -Path $buildPath -Force
            
            # Copiar os arquivos gerados para o diretório
            Copy-Item -Path "$buildDir\*.dll" -Destination $buildPath -Force
            Copy-Item -Path "$buildDir\*.exe" -Destination $buildPath -Force

            # Empacotar o diretório em um arquivo ZIP
            $zipPath = "$buildDir\$projectName-$build-$conf-$version-(MSVC).zip"
            Compress-Archive -Path "$buildPath\*" -DestinationPath $zipPath -Force
        }
    }
}