# Systemressursovervåker

## Oversikt
Systemressursovervåker er et skrivebordsprogram for Linux som overvåker systemressurser som CPU-bruk, minnebruk og diskplass. Programmet viser ressursbruk i sanntid med grafiske diagrammer og varsler brukeren når ressursbruken når kritiske nivåer.

## Hovedfunksjoner
- Visuell overvåking av CPU-bruk med historikk
- Visuell overvåking av minnebruk med historikk
- Diskbrukanalyse for alle tilkoblede lagringsenheter
- Sanntidsvarsler når ressursbruken overstiger definerte terskler
- Fullt lokalisert brukergrensesnitt på russisk

## Systemkrav
- Linux-operativsystem
- GTK+3.0-bibliotek
- libnotify (for varsler)
- Minimum 100 MB ledig diskplass
- 512 MB RAM

## Installasjon

### Metode 1: Fra kildefiler
```bash
# Klone kildekodearkivet
git clone https://github.com/assab78/System-Monitor-C-.git
cd System-Monitor-C

# Installer avhengigheter
sudo apt-get install build-essential libgtk-3-dev libnotify-dev pkg-config

# Bygg programmet
make

# Kjør programmet
./bin/system-monitor
```

### Metode 2: Med installasjonsskript
```bash
# Last ned kildekode
git clone System-Monitor-C.git
cd System-Monitor-C

# Kjør installasjonsskriptet
sudo ./install.sh
```

## Bruk
Start programmet fra programmeny eller fra terminalen:
```bash
system-monitor
```

Programmet har to hovedfaner:
1. **Overvåking** - Viser sanntidsgrafer for systemressurser
2. **Innstillinger** - Lar deg konfigurere varselterskler og andre parametere

### Varsler
Programmet vil vise varsler når:
- CPU-bruk overstiger 85% i mer enn 30 sekunder
- Minnebruk overstiger 85%
- Diskplass på hvilken som helst partisjon overstiger 90%

### Kjente problemer
- I noen containermiljøer som Replit kan diskdelen være tom fordi noen systempartisjoner filtreres bort
- Varselmekanismen fungerer kanskje ikke riktig når D-Bus-tjenesten ikke er tilgjengelig

## Konfigurasjon
Programmet lagrer innstillinger i `~/.config/system-monitor/settings.conf`. 
Du kan manuelt redigere denne filen for å endre varselterskler og oppdateringsfrekvens.

```
# Eksempel på konfigurasjonsfil
cpu_threshold=85        # CPU-varselterskel (%)
memory_threshold=85     # Minnevarselterskel (%)
disk_threshold=90       # Diskvarselterskel (%)
update_interval=1000    # Oppdateringsintervall (ms)
notification_cooldown=300 # Nedkjølingsperiode for varsler (sekunder)
```

## Utvikling
Prosjektet er strukturert som følger:
- `src/` - Kildekode
- `assets/` - Bilder og ikoner
- `build/` - Kompilerte objektfiler
- `bin/` - Den ferdige kjørbare filen

### Komponentoversikt
- `main.cpp` - Programmets startpunkt
- `main_window.cpp` - Hovedvindu og UI-komponenter
- `resource_monitor.cpp` - Ressursovervåkingslogikk
- `resource_graphs.cpp` - Grafkomponenter for visualisering
- `notification_manager.cpp` - Håndterer systemvarsler
- `history_data.cpp` - Datalagring for historikk
- `settings.cpp` - Konfigurasjonshåndtering

## Lisens
Dette prosjektet er lisensiert under GNU General Public License v3.0.
