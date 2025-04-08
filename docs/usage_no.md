# Brukerveiledning for Systemressursovervåker

Denne veiledningen forklarer hvordan du bruker Systemressursovervåker for å overvåke og administrere ressursene på Linux-systemet ditt.

## Starte programmet

Du kan starte Systemressursovervåker på følgende måter:

1. **Fra programmenyen**: Finn programmet i programmeny under kategorien "Systemverktøy" eller "Overvåking".

2. **Fra terminalen**: Åpne en terminal og skriv:
   ```bash
   system-monitor
   ```
   
   Hvis du kjører programmet fra kildemappen, bruk:
   ```bash
   ./bin/system-monitor
   ```

## Hovedvinduet

Når programmet starter, vises hovedvinduet med tre hovedpaneler:

### 1. CPU-overvåking

Dette panelet viser:
- Gjeldende CPU-bruk som prosentandel
- Grafisk fremstilling av CPU-belastning over tid (siste 10 minutter)
- Kjernebruk for hver enkelt CPU-kjerne

Fargen på grafen indikerer belastningsnivå:
- **Grønn**: Normal belastning (0-70%)
- **Gul**: Høy belastning (70-85%)
- **Rød**: Kritisk belastning (over 85%)

### 2. Minneovervåking

Dette panelet viser:
- Total mengde fysisk minne (RAM)
- Brukt minne (i GB og prosent)
- Grafisk fremstilling av minnebruk over tid
- Veksleminnestatus (swap)

### 3. Diskovervåking

Dette panelet viser:
- Liste over alle tilkoblede disker og partisjoner
- Monteringspunkt for hver partisjon
- Total størrelse og tilgjengelig plass
- Grafisk fremstilling av diskbruk for hver partisjon

Du kan sortere diskene etter:
- Navn
- Monteringspunkt
- Brukt plass (%)
- Tilgjengelig plass

## Varsler

Systemressursovervåker gir deg varsler når systemressursene når kritiske nivåer:

- **CPU-varsler**: Vises når CPU-bruken overstiger 85% i mer enn 30 sekunder
- **Minnevarsler**: Vises når minnebruken overstiger 85% av totalt tilgjengelig minne
- **Diskvarsler**: Vises når en diskpartisjon har mindre enn 10% ledig plass

Varsler vises som systemnotifikasjoner og har en nedkjølingsperiode på 5 minutter for å unngå for mange gjentatte varsler.

## Innstillinger

Du kan tilpasse programmets innstillinger ved å klikke på "Innstillinger"-knappen i verktøylinjen. Dette åpner en ny dialog med følgende alternativer:

### Generelle innstillinger
- **Oppdateringsintervall**: Hvor ofte grafikken oppdateres (i millisekunder)
- **Start automatisk ved pålogging**: Starter programmet automatisk når du logger på

### Varslingsinnstillinger
- **CPU-varselterskel**: Prosentandel for når CPU-varsler utløses
- **Minnevarselterskel**: Prosentandel for når minnevarsler utløses
- **Diskvarselterskel**: Prosentandel for når diskvarsler utløses
- **Nedkjølingsperiode**: Tid mellom varsler av samme type (i sekunder)

### Visningsinnstillinger
- **Vis tray-ikon**: Viser et ikon i systemets varslingsfelt
- **Minimer til tray ved lukking**: Minimerer programmet til varslingsfelt i stedet for å avslutte
- **Tema**: Velg mellom lyst, mørkt eller systemtema

## Grafisk historikk

Systemressursovervåker samler inn data kontinuerlig og viser historikk for de siste 10 minuttene. Dette gir deg mulighet til å se trender og mønstre i ressursbruken.

For å se mer detaljert historikk, hold musepekeren over en bestemt del av grafen. En verktøytips vil vise nøyaktig verdi og tidspunkt.

## Hurtigtaster

Programmet støtter følgende hurtigtaster:

| Tast | Funksjon |
|------|----------|
| F5 | Oppdater alle data |
| Ctrl+S | Åpne innstillingsdialogen |
| Ctrl+Q | Avslutt programmet |
| F1 | Vis hjelp |
| Ctrl+M | Minimer til varslingsfelt |

## Avansert bruk

### Kommandolinjealternativer

Programmet støtter flere kommandolinjealternativer:

```bash
system-monitor [alternativer]
```

Tilgjengelige alternativer:
- `--help` eller `-h`: Vis hjelpeinformasjon
- `--version` eller `-v`: Vis versjonsinformasjon
- `--minimize` eller `-m`: Start minimert i varslingsfelt
- `--config=FILE` eller `-c FILE`: Bruk en alternativ konfigurasjonsfil

### Eksportere data

Du kan eksportere systemressursdata ved å høyreklikke på en graf og velge "Eksporter data". Dette vil la deg lagre dataene som CSV-fil for videre analyse i andre programmer.