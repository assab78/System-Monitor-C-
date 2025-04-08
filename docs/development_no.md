# Utviklerveiledning for Systemressursovervåker

Denne veiledningen er for utviklere som ønsker å bidra til Systemressursovervåker-prosjektet eller tilpasse programmet til egne behov.

## Prosjektstruktur

Systemressursovervåker følger en modulær struktur for å gjøre koden lettere å vedlikeholde og utvide:

```
systemressursovervaker/
├── assets/                # Ikoner og grafiske elementer
├── bin/                   # Kompilerte kjørbare filer
├── build/                 # Midlertidige objektfiler under kompilering
├── docs/                  # Dokumentasjon
├── src/                   # Kildekode
│   ├── history_data.cpp   # Datalagring for historikk
│   ├── history_data.h
│   ├── main.cpp           # Applikasjonens hovedinngang
│   ├── main_window.cpp    # Hovedvindu og brukergrensesnitt
│   ├── main_window.h
│   ├── notification_manager.cpp  # Varslingssystem
│   ├── notification_manager.h
│   ├── resource_graphs.cpp  # Grafikkomponenter for visualisering
│   ├── resource_graphs.h
│   ├── resource_monitor.cpp  # Systemressursovervåking
│   ├── resource_monitor.h
│   ├── settings.cpp       # Konfigurasjonshåndtering
│   └── settings.h
├── Makefile               # Byggeskript
├── README.md              # Engelsk dokumentasjon
└── README.no.md           # Norsk dokumentasjon
```

## Avhengigheter

Prosjektet krever følgende biblioteker:

- **GTK+ 3.0**: Grafisk brukergrensesnitt
- **Cairo**: Tegning av grafer og diagrammer
- **libnotify**: Systemvarsler
- **glib-2.0**: Generelle verktøy
- **procps**: Grensesnitt til prosessinformasjon

## Byggesystem

Prosjektet bruker et standard GNU Makefile for kompilering. Hovedmål i Makefile:

- `make`: Bygg programmet
- `make clean`: Fjern kompilerte filer
- `make install`: Installer programmet på systemet
- `make uninstall`: Avinstaller programmet
- `make doc`: Generer dokumentasjon (krever Doxygen)

## Hovedkomponenter

### ResourceMonitor

`ResourceMonitor`-klassen er ansvarlig for å samle systemressursdata. Den bruker systemspesifikke API-er for å hente CPU, minne- og diskinformasjon.

```cpp
// Eksempel på bruk av ResourceMonitor
ResourceMonitor monitor;
double cpu_usage = monitor.get_cpu_usage();
double memory_usage = monitor.get_memory_usage();
std::vector<DiskInfo> disks = monitor.get_disk_info();
```

### NotificationManager

`NotificationManager` håndterer systemvarsler basert på ressursbruksterskler.

```cpp
// Eksempel på bruk av NotificationManager
NotificationManager notifications;
notifications.set_threshold(ResourceType::CPU, 85.0);
notifications.set_threshold(ResourceType::MEMORY, 85.0);
notifications.set_threshold(ResourceType::DISK, 90.0);

// Sjekk og vis varsler ved behov
notifications.check_and_notify(ResourceType::CPU, cpu_usage);
```

### HistoryData

`HistoryData` lagrer historiske ressursbruksdata for grafvisning.

```cpp
// Eksempel på bruk av HistoryData
HistoryData history(600); // Lagre 600 sekunder med data
history.add_data_point(ResourceType::CPU, cpu_usage);

// Hent historiske data for grafen
std::vector<double> cpu_history = history.get_data(ResourceType::CPU);
```

### ResourceGraphs

`ResourceGraphs` inneholder ulike grafkomponenter for visualisering av ressursbruk.

```cpp
// Eksempel på å opprette en CPU-graf
CpuGraphPanel* cpu_graph = new CpuGraphPanel(history);
gtk_container_add(GTK_CONTAINER(container), cpu_graph->get_widget());

// Oppdater grafen
cpu_graph->update();
```

### Settings

`Settings`-klassen håndterer lagring og lasting av brukerpreferanser.

```cpp
// Eksempel på bruk av Settings
Settings settings;
settings.load(); // Last fra fil

// Hent og sett verdier
double cpu_threshold = settings.get_double("cpu_threshold", 85.0);
settings.set_double("cpu_threshold", 90.0);

// Lagre endringer
settings.save();
```

## Grensesnittdesign

Brukergrensesnittet er designet med GTK+3 og følger GNOME Human Interface Guidelines. Hovedvinduet er organisert med en notebook-widget med separate faner for ulike ressurstyper.

Grafer tegnes ved hjelp av Cairo, som gir fleksibilitet i visualisering og tilpasning.

## Utvidelsesområder

Her er noen områder der prosjektet kan utvides:

### Nye funksjoner
- Nettverksovervåking
- Prosessovervåking (topp prosesser etter ressursbruk)
- Temperaturovervåking for CPU og GPU
- Loggføring av ressursbruk over lengre perioder
- Eksport av statistikk til ulike formater (CSV, JSON)

### Forbedringer
- Ytterligere støtte for nattmodus / tema
- Ytterligere lokaliseringsalternativer
- Mer detaljert CPU-informasjon (per kjerne, per tråd)
- Bedre grafstyling og visualiseringer
- Optimaliseringer for lavere ressursbruk

## Feilsøkingstips for utviklere

### Debugging

Prosjektet støtter debug-bygging med ekstra loggføring:

```bash
make DEBUG=1
```

Dette aktiverer debug-symboler og ekstra loggmeldinger under kjøring.

For å vise detaljerte logger under kjøring:

```bash
G_MESSAGES_DEBUG=all ./bin/system-monitor
```

### Vanlige feil

1. **GTK-feil**: Hvis du får segmenteringsfeil relatert til GTK, sjekk at alle GUI-oppdateringer gjøres fra hovedtråden.

2. **Høy CPU-bruk**: Hvis selve monitoren bruker for mye CPU, juster oppdateringsintervallet eller implementer throttling.

3. **Manglende varsler**: Sjekk at D-Bus-tjenesten kjører og at libnotify er riktig konfigurert.

## Bidrag

Bidrag til prosjektet er velkomne! For å bidra:

1. Fork prosjektet på GitHub
2. Lag en gren for din funksjon (`git checkout -b feature/min-nye-funksjon`)
3. Commit dine endringer (`git commit -am 'Legg til min nye funksjon'`)
4. Push grenen (`git push origin feature/min-nye-funksjon`)
5. Opprett en Pull Request

### Kodestil

Prosjektet følger disse generelle kodestilreglene:

- Bruk camelCase for variabel- og metodenavn
- Bruk PascalCase for klassenavn
- Bruk understrek_navngivning for filnavn
- Innrykk med 4 mellomrom, ikke tabulatorer
- Maksimal linjelengde på 100 tegn
- Dokumenter alle offentlige funksjoner og klasser

## Testramme

Enhetstest er implementert med Google Test. For å kjøre testene:

```bash
make test
./bin/unit_tests
```

## Ytelsesprofilering

For å profilere applikasjonens ytelse, bruk valgrind:

```bash
valgrind --tool=callgrind ./bin/system-monitor
```

For minnelekkasjesjekking:

```bash
valgrind --leak-check=full ./bin/system-monitor
```