# 05 — Demo cronometrata + video backup

Stima: **4-5h**. Pre-requisito: aver letto [`06_pratica_script.md`](06_pratica_script.md) per allineare i comandi della demo con le frasi-pivot delle slide 10 e 11.

## Setup terminale (10 min, una sola volta)

- Terminale dedicato: prompt corto (es. `$`), font ≥ 18pt, sfondo chiaro
- Tema con contrasto alto (no Solarized scuro)
- Ingrandisci la finestra a fullscreen, lascia spazio bianco sopra
- Pre-popolare `~/.bash_history` con i comandi della demo nell'ordine giusto
  (basta freccia ↑ durante il seminario, niente typo in diretta)
- Disabilita notifiche di sistema (Slack, Teams, Mail)

## Comandi demo (ordine)

Da [`Martin/1_clang-tidy_custom_check/SCRIPT.md`](../Martin/1_clang-tidy_custom_check/SCRIPT.md) e [`Martin/1_clang-tidy_custom_check/demo/`](../Martin/1_clang-tidy_custom_check/demo/):

1. Mostra il check installato: `clang-tidy --list-checks | grep no-printf`
2. Run su snippet semplice: cattura il warning con location + nome check
3. Run su `tinyxml2` (o output preregistrato): "28 hit, 0 falsi positivi"
4. Bear demo: `bear -- make` → `compile_commands.json` → run clang-tidy
5. Confronto LLM: aprire un terminale affianco con la risposta dello snippet 06 (overload resolution)

## Pratica (4h)

- **Run 1-2** (warm-up, ~30 min): leggi le frasi-pivot mentre digiti, niente cronometro. Obiettivo: zero errori di battitura.
- **Run 3-4** (~30 min): cronometra. Target sezione "demo live" = **4-5 min**. Annota dove vai lungo.
- **Run 5+** (~30 min): full run dell'intero seminario (slide + demo + Q&A finta). Target totale = **15 min**.

Se a Run 5 sei sopra 16 min: taglia. Se sotto 13 min: stai correndo, rallenta sui punti tecnici.

## Piano B — video backup (1h)

Se la live si rompe (kernel panic, terminale che non risponde, demo che fallisce su VM diversa), parti il video.

- QuickTime → File → New Screen Recording
- Solo la finestra del terminale (non lo schermo intero, distrae)
- Audio dal microfono **off** (ci metti la voce in diretta sopra)
- Target: **2'30"** condensato. Salta i dettagli, lascia solo i 3 momenti wow:
  1. Warning su snippet semplice
  2. Hit su tinyxml2
  3. LLM che sbaglia lo snippet 06
- Salva come `Martin/1_clang-tidy_custom_check/demo/backup.mov`
- Test del backup: aprire il file, far andare a partire dal Finder con doppio click (deve funzionare anche senza QuickTime in foreground)

## Checklist giorno del seminario

- [ ] Terminale aperto con cwd giusta
- [ ] `~/.bash_history` con i comandi pronti (freccia ↑)
- [ ] File `backup.mov` aperto in tab/finestra nascosta
- [ ] Slide aperte in modalità presentazione
- [ ] Notifiche silenziate
- [ ] Acqua a portata di mano
