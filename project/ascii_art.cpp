//
// Projekt kasutab stb_image.h library, et sisendid muuta ASCII kunstiks.
// Selles .cpp failis on programmi loogika selle alusel.
//
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_resize2.h"
#include "gif_load.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <iomanip>
using namespace std;

// Kasutatavad ASCII sümbolid, saab täiendada
const char *ascii_chars = "@%#*+=-:. ";

// Meetod, mis muudab piksli grayscale värvi, otsustab selle järgi indeksi ascii_chars muutujast
// ja tagastab selle piksli valitud ASCII sümboli.
char pixelToChar(unsigned char r, unsigned char g, unsigned char b) {
    float gray = 0.299f * r + 0.587f * g + 0.114f * b;
    int index = (gray / 255.0f) * (strlen(ascii_chars) - 1);
    return ascii_chars[index];
}

struct AsciiFrame {
    vector<string> lines;
    int delay_ms;
};

struct GifAnim {
    int width, height;
    vector<AsciiFrame> frames;
};


//GIF meetod
void gifFrameWriter(void *user, struct GIF_WHDR *whdr) {
    static vector<uint8_t> canvas;
    static vector<uint8_t> prev_canvas;

    GifAnim *anim = static_cast<GifAnim *>(user);
    int full_w = whdr->xdim;
    int full_h = whdr->ydim;
    int out_width = anim->width;
    int out_height = anim->height;


    if (canvas.empty()) {
        canvas.resize(full_w * full_h * 3, 0);
        prev_canvas.resize(canvas.size());
    }

    if (whdr->mode == GIF_PREV) {
        prev_canvas = canvas;
    }

    for (int y = 0; y < whdr->fryd; ++y) {
        for (int x = 0; x < whdr->frxd; ++x) {
            int src_index = y * whdr->frxd + x;
            int dst_x = x + whdr->frxo;
            int dst_y = y + whdr->fryo;
            int dst_index = (dst_y * full_w + dst_x) * 3;

            int palette_index = whdr->bptr[src_index];
            if (palette_index == whdr->tran) continue;

            canvas[dst_index + 0] = whdr->cpal[palette_index].R;
            canvas[dst_index + 1] = whdr->cpal[palette_index].G;
            canvas[dst_index + 2] = whdr->cpal[palette_index].B;
        }
    }

    unsigned char *resized = new unsigned char[out_width * out_height * 3];
    stbir_resize_uint8_srgb(canvas.data(), full_w, full_h, 0,
                            resized, out_width, out_height, 0, STBIR_RGB);

    AsciiFrame frame;
    frame.delay_ms = whdr->time * 10;

    for (int y = 0; y < out_height; y += 5) {
        string line;
        for (int x = 0; x < out_width; x += 2) {
            int i = (y * out_width + x) * 3;
            char c = pixelToChar(resized[i], resized[i + 1], resized[i + 2]);
            line += c;
        }
        frame.lines.push_back(line);
    }

    anim->frames.push_back(frame);
    delete[] resized;

    if (whdr->mode == GIF_BKGD) {
        for (int y = 0; y < whdr->fryd; ++y) {
            for (int x = 0; x < whdr->frxd; ++x) {
                int dst_x = x + whdr->frxo;
                int dst_y = y + whdr->fryo;
                int dst_index = (dst_y * full_w + dst_x) * 3;
                canvas[dst_index + 0] = 0;
                canvas[dst_index + 1] = 0;
                canvas[dst_index + 2] = 0;
            }
        }
    } else if (whdr->mode == GIF_PREV) {
        canvas = prev_canvas;
    }
}


// Näitab töö kestvust (täiustav UI element)
void showProgress(int current, int total) {
    const int barWidth = 40;
    float progress = (float) current / total;
    cout << "\t[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) cout << "=";
        else if (i == pos) cout << ">";
        else cout << " ";
    }
    cout << "] " << setw(3) << int(progress * 100.0) << "%\r";
    cout.flush();
}

//
void title() {
    cout << "\n\n\t\t╔══════════════════════════════════════╗\n";
    cout << "\t\t║            ASCII GEN v1.0.1          ║\n";
    cout << "\t\t║                                      ║\n";
    cout << "\t\t║   Mattias Antsov, Gregor Artur Mäe   ║\n";
    cout << "\t\t╚══════════════════════════════════════╝\n\n";
}


int main() {
    int width, height, channels;
    string in_filename, out_filename;
    title();

    cout << "\tPalun sisesta failinimi (koos failimuutujaga): ";
    cin >> in_filename;

    // Kontrollib faili olemasolu, mac versioonil annab errori
    /**
    if (!filesystem::exists(in_filename)) {
        cerr << "\tFaili \"" << in_filename << "\" ei leitud.\n";
        return 1;
    }
    */

    bool isGif = in_filename.size() >= 4 &&
                 (in_filename.substr(in_filename.size() - 4) == ".gif" ||
                  in_filename.substr(in_filename.size() - 4) == ".GIF");


    // Laeb pikslite andmed ühte muutujasse, kontrollib GIF struktuuri
    if (!isGif) {
        unsigned char *probe = stbi_load(in_filename.c_str(), &width, &height, &channels, 3);
        if (!probe) {
            cerr << "\tVigane sisend. Kas fail on pilt?\n";
            return 1;
        }
        stbi_image_free(probe);
    } else {
        width = 360;
        height = 240;
    }

    // Dünaamiline laius ja kõrgus ning selle kuvamine
    float aspect_ratio = 0.75f;
    int out_width = max(min(width / 4, 240), 60);
    int out_height = static_cast<int>((height * out_width * aspect_ratio) / width);

    cout << "\n\tArvutatud mõõtmed: " << out_width << " x " << out_height
         << " (laius x kõrgus) – sobib enamikes tekstivaatajates\n";

    // Laiuse/kõrguse määramise küsimine
    // Kuna ASCII sümbolid pole ruudud (nagu on piksel), tuleb pikslite andmed muuta väiksemaks stb resize meetodiga,
    // vastasel juhul vaikeparameetriga ilmuksid sümbolid vertikaalselt teineteise peal ning ASCII kunsti poleks näha
    string choice;
    cout << "\tKas soovid määrata laiuse ja kõrguse ise? [Y/N]: ";
    cin >> choice;
    if (choice == "y" || choice == "Y") {
        cout << "\n\tSisesta soovitud laius (nt 480): ";
        cin >> out_width;
        cout << "\tSisesta soovitud kõrgus (nt 360): ";
        cin >> out_height;
        if (out_width <= 0 || out_height <= 0) {
            cerr << "\tVigased mõõdud. Programm katkestub.\n";
            return 1;
        }
    }
    cin.ignore();

    // Output faili nime küsimine, jätab samaks mis sisend kui jäetakse vahele
    cout << "\n\tSisesta väljundfaili nimi: ";
    getline(cin, out_filename);
    if (out_filename.empty()) {
        size_t dotPos = in_filename.find_last_of('.');
        out_filename = (dotPos != string::npos ? in_filename.substr(0, dotPos) : in_filename) + ".txt";
    } else if (out_filename.find(".txt") == string::npos) {
        out_filename += ".txt";
    }

    if (!isGif) {
        unsigned char *data = stbi_load(in_filename.c_str(), &width, &height, &channels, 3);
        if (!data) {
            cerr << "\tVigane sisend. Kas fail on pilt?\n";
            return 1;
        }

        unsigned char *resized = new unsigned char[out_width * out_height * 3];
        stbir_resize_uint8_srgb(data, width, height, 0, resized, out_width, out_height, 0, STBIR_RGB);
        stbi_image_free(data);

        ofstream out(out_filename);
        int last_percent = -1;
        for (int y = 0; y < out_height; y += 5) {
            for (int x = 0; x < out_width; x += 2) {
                int i = (y * out_width + x) * 3;
                char c = pixelToChar(resized[i], resized[i + 1], resized[i + 2]);
                out << c;
            }
            out << '\n';
            int current_percent = (100 * (y + 1)) / out_height;
            if (current_percent != last_percent) {
                showProgress(y + 1, out_height);
                last_percent = current_percent;
            }
        }
        out.close();
        delete[] resized;
        cout << "\t[========================================] 100%\n";
        cout << "\n\tProgramm on töö edukalt lõpetanud! Tulemus on nähtav root kaustas.\n";
        return 0;
    }

    // GIF integratsioon
    GifAnim anim;
    anim.width = out_width;
    anim.height = out_height;

    ifstream gifFile(in_filename, ios::binary);
    vector<uint8_t> gifData((istreambuf_iterator<char>(gifFile)), istreambuf_iterator<char>());

    long frameCount = GIF_Load(gifData.data(), gifData.size(), gifFrameWriter, nullptr, &anim, 0);
    if (frameCount <= 0) {
        cerr << "\tGIF laadimine ebaõnnestus.\n";
        return 1;
    }

    cout << "\n\tKaadreid leitud: " << frameCount << "\n";

    char animate;
    cout << "\tKas soovid näidata animatsiooni terminalis? [Y/N]: ";
    cin >> animate;
    cin.ignore();

    if (animate == 'y' || animate == 'Y') {
        for (const auto &frame : anim.frames) {
            system("clear");
            for (const auto &line : frame.lines)
                cout << line << '\n';
            this_thread::sleep_for(chrono::milliseconds(frame.delay_ms));
        }
    } else {
        for (size_t i = 0; i < anim.frames.size(); ++i) {
            char fname[64];
            sprintf(fname, "frame_%03zu.txt", i + 1);
            ofstream out(fname);
            for (const auto &line : anim.frames[i].lines)
                out << line << '\n';
            out.close();
        }
        cout << "\n\tKaadrid salvestatud kujul frame_###.txt!\n";
    }

    return 0;
}
