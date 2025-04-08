# Feilsøkingsveiledning for Systemressursovervåker

Denne veiledningen hjelper deg med å løse vanlige problemer med Systemressursovervåker-applikasjonen.

## Generelle problemer

### Programmet starter ikke

**Symptom:** Ingenting skjer når du prøver å starte programmet, eller det krasjer umiddelbart.

**Mulige løsninger:**

1. **Manglende avhengigheter**: Kontroller at alle nødvendige biblioteker er installert:
   ```bash
   sudo apt-get install libgtk-3-0 libnotify4
   ```

2. **Rettigheter**: Kontroller at programfilen har kjørerettigheter:
   ```bash
   chmod +x /path/to/system-monitor
   ```

3. **Filkorrupsjon**: Prøv å installere programmet på nytt:
   ```bash
   make clean && make
   ```

4. **Bibliotekskonflikt**: Sjekk om det er bibliotekskonflikter:
   ```bash
   ldd bin/system-monitor | grep "not found"
   ```

### Høy CPU-bruk fra selve programmet

**Symptom:** Systemressursovervåker bruker selv mye CPU-ressurser.

**Mulige løsninger:**

1. **Oppdateringsintervall**: Juster oppdateringsintervallet i innstillingene til en høyere verdi (f.eks. 2000 ms i stedet for 1000 ms).

2. **Grafisk akselerasjon**: Kontroller at systemet ditt har riktige grafikkdrivere installert for bedre ytelse.

3. **Begrenset historikk**: Reduser mengden historikk som lagres ved å redigere konfigurasjonsfilen:
   ```
   history_size=300  # Reduser fra standardverdien på 600
   ```

4. **For mange disker**: Hvis du har mange monteringspunkter, vurder å filtrere noen av dem i innstillingene.

## Visningsproblemer

### Tomme eller manglende grafer

**Symptom:** En eller flere av grafene vises ikke eller er tomme.

**Mulige løsninger:**

1. **Diskgraf mangler**: I containeriserte miljøer som Replit kan diskgrafer virke tomme på grunn av begrensninger i containerens tilgang til underliggende filsystem.

2. **Manglende datapunkter**: Restart programmet og vent noen sekunder for at historikkdata skal samles inn.

3. **Filtreringsproblemer**: Sjekk om filtreringsinnstillingene ekskluderer enhetene dine:
   ```bash
   nano ~/.config/system-monitor/settings.conf
   # Sjekk eller endre 'disk_filter_patterns' innstillingen
   ```

### Forvrengede grafer eller tekst

**Symptom:** Grafer eller tekst ser forvrengt ut eller er ikke lesbar.

**Mulige løsninger:**

1. **Temakonflikter**: Prøv å bytte tema i innstillingene eller bruk standardtemaet.

2. **Skalering**: For HiDPI-skjermer, prøv å starte programmet med skalering:
   ```bash
   GDK_SCALE=2 ./bin/system-monitor
   ```

3. **Fontproblemer**: Sjekk om systemfontene er riktig installert:
   ```bash
   fc-cache -f -v
   ```

## Varslingsproblemer

### Ingen varsler vises

**Symptom:** Programmet kjører, men du får ingen varsler når ressursbruken er høy.

**Mulige løsninger:**

1. **D-Bus-problem**: Kontroller at D-Bus-tjenesten kjører:
   ```bash
   systemctl status dbus
   ```

2. **Manglende notifikasjonsdaemon**: Sjekk om en notifikasjonsdaemon kjører i skrivebordsmiljøet ditt.

3. **Høye terskelverdier**: Juster terskelverdiene i innstillingene til lavere verdier (f.eks. 80% i stedet for 85%).

4. **Deaktivert varslinger**: Kontroller at varsler ikke er deaktivert i innstillingene:
   ```bash
   nano ~/.config/system-monitor/settings.conf
   # Sjekk om 'notifications_enabled=true'
   ```

### For mange varsler

**Symptom:** Du mottar for mange gjentatte varsler om samme problem.

**Mulige løsninger:**

1. **Nedkjølingsperiode**: Øk nedkjølingsperioden i innstillingene:
   ```
   notification_cooldown=600  # Øk til 10 minutter
   ```

2. **Terskeljustering**: Øk terskelverdiene for å redusere antall varsler:
   ```
   cpu_threshold=90
   memory_threshold=90
   disk_threshold=95
   ```

## Driftsproblemer

### Programmet fryser periodisk

**Symptom:** Brukergrensesnittet fryser i korte perioder.

**Mulige løsninger:**

1. **Trådproblemer**: Sjekk systemlogger for mulige deadlocks:
   ```bash
   journalctl | grep system-monitor
   ```

2. **I/O-ventetid**: På systemer med trege disker kan diskstatistikk forårsake forsinkelser. Prøv å deaktivere diskvisning i innstillingene.

3. **Minnelekkasje**: Hvis programmet bruker stadig mer minne, restart det regelmessig eller oppgrader til nyeste versjon.

### Programmet respekterer ikke nedkjølingsperioden for varslinger

**Symptom:** Du mottar varsler oftere enn nedkjølingsperioden tilsier.

**Mulige løsninger:**

1. **Separate nedkjølingsperioder**: I nyere versjoner brukes separate nedkjølingsperioder for ulike ressurstyper. Sjekk at du bruker siste versjon av programmet.

2. **Konfigurasjonsproblemer**: Slett konfigurasjonsfilen og la programmet opprette en ny med standardverdier:
   ```bash
   rm ~/.config/system-monitor/settings.conf
   ```

## Plattformspesifikke problemer

### Replit og containeriserte miljøer

I containeriserte miljøer som Replit kan du oppleve følgende begrensninger:

1. **Tomme diskgrafer**: Dette skyldes begrensninger i containermiljøet. Prøv å kjøre med flagget:
   ```bash
   ./system-monitor --disable-disk-filters
   ```

2. **Manglende varsler**: Varsler kan mangle fordi D-Bus ofte ikke er tilgjengelig i containere.

3. **Begrenset ressurstilgang**: Containeriserte miljøer gir begrenset informasjon om det fysiske systemet.

### Virtualiserte miljøer (VM)

I virtuelle maskiner:

1. **Unøyaktige maskinvareinformasjon**: CPU-modell og annen maskinvareinformasjon kan ikke være nøyaktig.

2. **Begrenset tilgang til sensorer**: Temperatur- og viftehastighetsovervåking er muligens ikke tilgjengelig.

## Logginformasjon

For å aktivere utvidet logging for feilsøking, start programmet slik:

```bash
G_MESSAGES_DEBUG=all ./bin/system-monitor 2> system-monitor-debug.log
```

Dette vil opprette en loggfil med detaljert feilsøkingsinformasjon som kan være nyttig for å diagnostisere problemer.

## Kontakt og støtte

Hvis du fortsatt har problemer etter å ha prøvd disse løsningene, kan du:

1. Sende en feilrapport med feilsøkingsloggen vedlagt
2. Sjekke GitHub-prosjektet for kjente problemer og løsninger
3. Kontakte utviklerne via support-e-postadressen

## Vanlige feilmeldinger og løsninger

| Feilmelding | Sannsynlig årsak | Løsning |
|-------------|------------------|---------|
| "Failed to initialize GTK" | Manglende GTK-biblioteker | Installer GTK+3 biblioteker |
| "Cannot connect to D-Bus" | D-Bus-tjenesten kjører ikke | Start D-Bus-tjenesten |
| "Permission denied when reading /proc" | Manglende rettigheter | Kjør med sudo eller gi riktige rettigheter |
| "Failed to load configuration" | Skadet konfigurasjonsfil | Slett konfigurasjonsfilen |
| "Disk information unavailable" | Begrenset tilgang i container | Bruk flagget --disable-disk-filters |