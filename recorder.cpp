#include <atomic>
#include <cstdint>
#include <iostream>
#include <portaudio.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

class AudioRecorder {
private:
  PaStream *inputStream;
  std::atomic<bool> stopRecord;

public:
  AudioRecorder() : stopRecord(false) {
    Pa_OpenDefaultStream(&inputStream, 1, 0, paInt16, 48000, 960, nullptr,
                         nullptr);
  }

  void listenKeyboard() {
    std::string enter;

    std::getline(std::cin, enter);

    stopRecord = true;
  }

  std::vector<int16_t> record() {
    std::vector<int16_t> audiosample;
    stopRecord = false;
    Pa_StartStream(inputStream);

    std::cout << "Recording...\nPress Enter to stop" << std::flush;

    int16_t buffer[960];

    std::thread listener(&AudioRecorder::listenKeyboard, this);
    while (!stopRecord) {
      Pa_ReadStream(inputStream, buffer, 960);

      audiosample.insert(audiosample.end(), buffer, buffer + 960);
    }

    Pa_StopStream(inputStream);
    listener.join();

    return audiosample;
  }

  ~AudioRecorder() { Pa_CloseStream(inputStream); }
};

class AudioPlayer {
private:
  PaStream *outputStream;

public:
  AudioPlayer() {
    Pa_OpenDefaultStream(&outputStream, 0, 1, paInt16, 48000, 960, nullptr,
                         nullptr);
  }

  void play(const std::vector<int16_t> &audiosample) {
    Pa_StartStream(outputStream);

    Pa_WriteStream(outputStream, audiosample.data(), audiosample.size());

    Pa_StopStream(outputStream);
  }

  ~AudioPlayer() { Pa_CloseStream(outputStream); }
};

class AudioApp {
private:
  AudioRecorder recorder;
  AudioPlayer player;
  std::unordered_map<std::string, std::vector<int16_t>> recordings;

public:
  AudioApp() {}

  void run() {
    std::cout << "Commands:\nTo record :record <audio_name>\nTo play audio: "
                 "play <audio_name>"
              << std::endl;
    while (true) {
      std::string input;

      std::getline(std::cin, input);
      size_t commandIndex = input.find(" ");

      if (input == "stop") {
        std::cout << "stopping the app..." << std::endl;
        break;
      }

      else if (commandIndex == std::string::npos) {
        std::cout << "Enter valid command";
        continue;
      }

      else {
        std::string commandType = input.substr(0, commandIndex);
        std::string filename = input.substr(commandIndex + 1);

        if (commandType == "record") {
          std::vector<int16_t> audio = recorder.record();
          recordings[filename] = audio;

          std::cout << "Recorded successfully!" << std::endl;
        }

        else if (commandType == "play") {
          if (recordings.find(filename) != recordings.end()) {
            player.play(recordings[filename]);
          }

          else {
            std::cout << "File not found!" << std::endl;
          }
        }

        else {
          std::cout << "Enter valid command" << std::endl;
        }
      }
    }
  }

  ~AudioApp() {}
};

int main() {
  Pa_Initialize();

  AudioApp app;

  app.run();

  Pa_Terminate();

  return 0;
}
