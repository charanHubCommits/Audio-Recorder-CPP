#include <atomic>
#include <cstdint>
#include <iostream>
#include <opus/opus.h>
#include <portaudio.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

struct AudioFile {
  std::vector<unsigned char> audioSample;
  std::vector<int> packetSizes;
};

class AudioRecorder {
private:
  PaStream *inputStream;
  OpusEncoder *enc;
  std::atomic<bool> stopRecord;

public:
  AudioRecorder() : stopRecord(false) {
    Pa_OpenDefaultStream(&inputStream, 1, 0, paInt16, 48000, 960, nullptr,
                         nullptr);
    enc = opus_encoder_create(48000, 1, OPUS_APPLICATION_AUDIO, NULL);
  }

  void listenKeyboard() {
    std::string enter;

    std::getline(std::cin, enter);

    stopRecord = true;
  }

  AudioFile record() {
    std::vector<unsigned char> audioSample;
    std::vector<int> packetSizes;

    stopRecord = false;
    Pa_StartStream(inputStream);

    std::cout << "Recording...\nPress Enter to stop" << std::flush;

    int16_t sampleBuffer[960];
    unsigned char compressedBuffer[960];

    std::thread listener(&AudioRecorder::listenKeyboard, this);
    while (!stopRecord) {
      Pa_ReadStream(inputStream, sampleBuffer, 960);

      int packetSize =
          opus_encode(enc, sampleBuffer, 960, compressedBuffer, 4000);

      packetSizes.push_back(packetSize);
      audioSample.insert(audioSample.end(), compressedBuffer,
                         compressedBuffer + packetSize);
    }

    Pa_StopStream(inputStream);
    listener.join();

    AudioFile audioFile;
    audioFile.audioSample = audioSample;
    audioFile.packetSizes = packetSizes;

    return audioFile;
  }

  ~AudioRecorder() {
    opus_encoder_destroy(enc);
    Pa_CloseStream(inputStream);
  }
};

class AudioPlayer {
private:
  PaStream *outputStream;
  OpusDecoder *dec;

public:
  AudioPlayer() {
    Pa_OpenDefaultStream(&outputStream, 0, 1, paInt16, 48000, 960, nullptr,
                         nullptr);
    dec = opus_decoder_create(48000, 1, NULL);
  }

  void play(const AudioFile &audioFile) {
    Pa_StartStream(outputStream);

    // Pa_WriteStream(outputStream, audiosample.data(), audiosample.size());

    // int sampleSize = audiosample.size();

    int16_t outputBuffer[960];
    int offset = 0;
    for (int size : audioFile.packetSizes) {
      int decodedSamples =
          opus_decode(dec, audioFile.audioSample.data() + offset, size,
                      outputBuffer, 960, 0);
      Pa_WriteStream(outputStream, outputBuffer, decodedSamples);

      offset += size;
    }

    Pa_StopStream(outputStream);
  }

  ~AudioPlayer() {
    Pa_CloseStream(outputStream);
    opus_decoder_destroy(dec);
  }
};

class AudioApp {
private:
  AudioRecorder recorder;
  AudioPlayer player;
  std::unordered_map<std::string, AudioFile> recordings;

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
          AudioFile audio = recorder.record();
          recordings[filename] = audio;

          std::cout << "Recorded successfully!" << std::endl;
        }

        else if (commandType == "play") {
          if (recordings.find(filename) != recordings.end()) {
            player.play(recordings[filename]);

            std::cout << "Audio play completed" << std::endl;
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
