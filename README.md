==========================================================================================
 ▗▄▖  ▗▄▄▖ ▗▄▄▖▗▄▄▄▖▗▄▄▄▖     ▗▄▖ ▗▄▄▖▗▄▄▄▖     ▗▄▄▖▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖▗▄▄▖  ▗▄▖▗▄▄▄▖▗▄▖ ▗▄▄▖
▐▌ ▐▌▐▌   ▐▌     █    █      ▐▌ ▐▌▐▌ ▐▌ █      ▐▌   ▐▌   ▐▛▚▖▐▌▐▌   ▐▌ ▐▌▐▌ ▐▌ █ ▐▌ ▐▌▐▌ ▐▌
▐▛▀▜▌ ▝▀▚▖▐▌     █    █      ▐▛▀▜▌▐▛▀▚▖ █      ▐▌▝▜▌▐▛▀▀▘▐▌ ▝▜▌▐▛▀▀▘▐▛▀▚▖▐▛▀▜▌ █ ▐▌ ▐▌▐▛▀▚▖
▐▌ ▐▌▗▄▄▞▘▝▚▄▄▖▗▄█▄▖▗▄█▄▖    ▐▌ ▐▌▐▌ ▐▌ █      ▝▚▄▞▘▐▙▄▄▖▐▌  ▐▌▐▙▄▄▖▐▌ ▐▌▐▌ ▐▌ █ ▝▚▄▞▘▐▌ ▐▌
===========================================================================================

Projekti liikmed: Mattias Antsov, Gregor Artur Mäe

MIDA TEEB?
"ASCII Art Generator" on täpselt nii nagu kõlab – see võtab sisendiks ühe pildi ja väljastab selle andmetest ühe tekstifaili, mis on ilusa ASCII kunsti sisuga.

KUIDAS TOIMIB?
Meie programm võtab sisendiks ühe pildifaili ning suunab selle läbi stb_image.h library meetodi, et saada sealt pildi iga piksli andmed.
Seejärel teisendatakse andmed väiksemasse formaati, sest vastasel juhul oleks tulemus vigane.
Peale seda töötleb programm iga piksli grayscale värvi ning valib iga piksli grayscale tooni järgi ühe vastava sümboli indeksi loogika järgi.
Selle töötluse tulemusena on iga muudetud piksel muudetud ASCII sümboliks ja sisestatud väljundfaili, mis on .txt kujul.
Programm sulgub ja projekti root kausta on tekkinud uus .txt fail, mis sisaldab ASCII kunsti, mis sarnaneb pildile.

KUIDAS KÄIVITADA?
Koodi käivitamiseks tuleb alguses minna programmi root kausta (IDE-s on see juba seal, terminalis tuleb kasutada "cd" käsku).
Seejärel saab käivitada kood käsuga "./ascii_art".
Programm küsib kasutajalt sisendfaili koos failimuutujaga ning seejärel tagastab tulemuse.

MIS ON TULEMUS?
Programm tagastab samanimelise faili .txt formaadis, mille sisu kujutab endast ilusat ASCII kunsti. :)
