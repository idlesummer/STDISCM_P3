<#
===========================================================
 GPT Bundle Script - Next.js Project
-----------------------------------------------------------
Bundles all human-readable source files into 
`docs/bundle.txt` with file headers for ChatGPT review.

▶ HOW TO RUN (PowerShell):
   1. cd path\to\NextProject
   2. powershell -ExecutionPolicy Bypass -File .\scripts\bundler.ps1

▶ Or use: `make bundle` if a Makefile is configured.
===========================================================
#>

# === CONFIG ===
$projectRoot = "./src"
$outputFile = "docs/bundle.txt"
$excludedExtensions = @('.exe', '.dll', '.obj', '.pdb', '.png', '.jpg', '.jpeg', '.gif', '.webp', '.ico', '.svg')

# Ensure output directory exists
New-Item -ItemType Directory -Force -Path (Split-Path $outputFile) | Out-Null

# === GPT Prompt ===
$gptPrompt = @"
--- Start of prompt ---
Use the FILE: and PATH: headers to locate the content.
--- End of prompt ---
"@

# Write the prompt first
$gptPrompt | Out-File $outputFile -Encoding UTF8

# === Collect Files ===
$projectFullPath = Resolve-Path $projectRoot
$files = Get-ChildItem -Path $projectFullPath -Recurse -File | Where-Object {
  # --- Exclude compiled/build/dependency folders ---
  $_.FullName -notlike "*\docs\*" -and

  # --- Exclude common lock/config/env files ---
  $_.Name -notmatch "package-lock\.json|pnpm-lock\.yaml|yarn\.lock|\.env.*" -and

  # --- Exclude binary/media files ---
  $excludedExtensions -notcontains $_.Extension
}

# === Append Each File ===
foreach ($file in $files) {
  $relativePath = $file.FullName.Substring($projectFullPath.Path.Length + 1)
  $header = @"
------------------------------
 FILE:  $($file.Name)
 PATH:  $relativePath
------------------------------
"@
  Add-Content -Path $outputFile -Value "`n$header`n"
  Get-Content $file.FullName -Raw | Add-Content -Path $outputFile
}

Write-Host "> Generated $outputFile with GPT prompt and $($files.Count) files."
