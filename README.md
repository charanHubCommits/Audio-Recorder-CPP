# projectAudio

A command-line audio recorder and player written in C++. It captures microphone input, stores recordings in memory by name, and plays them back through the default audio output device using [PortAudio](http://www.portaudio.com/).

## Features

- Record audio from the default input device (microphone)
- Play back named recordings through the default output device
- Interactive CLI with simple commands
- In-memory storage for multiple named recordings during a session

## Requirements

- C++ compiler with C++11 support (e.g. `g++` or `clang++`)
- [PortAudio](http://www.portaudio.com/) development libraries

### Installing PortAudio

**macOS (Homebrew):**

```bash
brew install portaudio
```

**Ubuntu/Debian:**

```bash
sudo apt install portaudio19-dev
```

**Fedora:**

```bash
sudo dnf install portaudio-devel
```

## Building

Compile from the project root:

```bash
g++ -std=c++11 recorder.cpp -o recorder -lportaudio -pthread
```

On macOS with Homebrew, you may need to pass include and library paths:

```bash
g++ -std=c++11 recorder.cpp -o recorder -I/opt/homebrew/include -L/opt/homebrew/lib -lportaudio -pthread
```

## Usage

Run the executable:

```bash
./recorder
```

Available commands:

| Command | Description |
|---------|-------------|
| `record <name>` | Start recording. Press Enter to stop. The recording is saved in memory under `<name>`. |
| `play <name>` | Play back a previously recorded clip. |
| `stop` | Exit the application. |

### Example

```
Commands:
To record :record <audio_name>
To play audio: play <audio_name>
record hello
Recording...
Press Enter to stop
Recorded successfully!
play hello
stop
```

## How It Works

The application is structured around three classes:

- **AudioRecorder** — Opens the default input stream (mono, 16-bit PCM, 48 kHz), reads audio in 960-frame chunks, and stops when Enter is pressed on a background thread.
- **AudioPlayer** — Opens the default output stream with matching format and writes the full sample buffer for playback.
- **AudioApp** — Runs the command loop and stores recordings in an in-memory map keyed by name.

Audio format: 48,000 Hz sample rate, 16-bit signed integer (`paInt16`), mono channel, 960 frames per buffer (~20 ms).

## Limitations

- Recordings are stored in memory only and are lost when the program exits.
- No support for saving to or loading from disk (e.g. WAV files).
- No volume control, pause, or seek during playback.
- Requires a working default input and output audio device.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
