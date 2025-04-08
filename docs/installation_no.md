# Installasjonsguide for Systemressursovervåker

Denne guiden forklarer hvordan du installerer Systemressursovervåker på Linux-systemet ditt.

## Forutsetninger

Før du begynner installasjonen, sørg for at systemet ditt oppfyller følgende krav:

- Et Linux-operativsystem
- Følgende pakker installert:
  - build-essential
  - libgtk-3-dev
  - libnotify-dev
  - pkg-config

Du kan installere disse pakkene på Ubuntu/Debian med følgende kommando:

```bash
sudo apt-get update
sudo apt-get install build-essential libgtk-3-dev libnotify-dev pkg-config
```

På Fedora:

```bash
sudo dnf install gcc-c++ gtk3-devel libnotify-devel pkg-config
```

På Arch Linux:

```bash
sudo pacman -S base-devel gtk3 libnotify pkg-config
```

## Installasjonsmetoder

### Metode 1: Automatisk installasjon (Anbefalt)

1. Last ned programmet og gå til programkatalogen:

```bash
git clone https://github.com/username/systemressursovervaker.git
cd systemressursovervaker
```

2. Kjør installasjonsskriptet (krever root-tilgang):

```bash
sudo ./install.sh
```

Dette skriptet vil:
- Installere nødvendige avhengigheter
- Kompilere programmet
- Installere det i `/usr/local/bin/`
- Opprette en skrivebordsfil for programmenyen
- Installere programikonet

3. Etter installasjonen kan du starte programmet fra programmenyen eller ved å kjøre:

```bash
system-monitor
```

### Metode 2: Manuell installasjon

1. Last ned programmet og gå til programkatalogen:

```bash
git clone https://github.com/username/systemressursovervaker.git
cd systemressursovervaker
```

2. Kompiler programmet:

```bash
make
```

3. Kjør programmet direkte fra mappen:

```bash
./bin/system-monitor
```

4. (Valgfritt) Hvis du vil installere programmet systemomfattende:

```bash
sudo cp bin/system-monitor /usr/local/bin/
sudo mkdir -p /usr/local/share/system-monitor/
sudo cp assets/application.svg /usr/local/share/system-monitor/
```

5. (Valgfritt) Opprett en skrivebordsfil for programmenyen:

```bash
sudo tee /usr/share/applications/system-resource-monitor.desktop > /dev/null << EOF
[Desktop Entry]
Name=Systemressursovervåker
Comment=Overvåk systemressurser og få varsler ved høy bruk
Exec=/usr/local/bin/system-monitor
Icon=/usr/local/share/system-monitor/application.svg
Terminal=false
Type=Application
Categories=System;Monitor;
Keywords=system;monitor;ressurser;cpu;minne;disk;
EOF
```

## Konfigurasjon etter installasjon

Programmet bruker en konfigurasjonsfil som lagres i `~/.config/system-monitor/settings.conf`. Denne filen vil bli opprettet automatisk første gang programmet kjøres.

Du kan tilpasse innstillingene ved å redigere denne filen med en teksteditor:

```bash
nano ~/.config/system-monitor/settings.conf
```

Eksempel på konfigurasjonsfil:

```
# Varselterskler (i prosent)
cpu_threshold=85
memory_threshold=85
disk_threshold=90

# Oppdateringsintervall i millisekunder
update_interval=1000

# Nedkjølingsperiode for varsler i sekunder
notification_cooldown=300
```

## Avinstallasjon

For å avinstallere programmet:

1. Hvis du installerte med installasjonsskriptet:

```bash
sudo rm /usr/local/bin/system-monitor
sudo rm -r /usr/local/share/system-monitor/
sudo rm /usr/share/applications/system-resource-monitor.desktop
```

2. For å fjerne brukerkonfigurasjonen:

```bash
rm -r ~/.config/system-monitor/
```

## Feilsøking

### Problem: Ingen varsler vises

Dette kan skyldes manglende D-Bus-tjenesteløp. Sørg for at D-Bus-tjenesten kjører:

```bash
systemctl status dbus
```

### Problem: Programmet starter ikke

Sjekk om alle avhengigheter er riktig installert med:

```bash
ldd bin/system-monitor | grep "not found"
```

Hvis dette viser manglende biblioteker, installer dem med pakkehåndtereren for distribusjonen din.

### Problem: Tomme grafikker for diskpartisjonene

I noen miljøer, spesielt containeriserte som Replit, kan diskfiltrene være for aggressive. Dette er en kjent begrensning i containermiljøer.