/*
  mkvmerge -- utility for splicing together matroska files
      from component media subtypes

  iso639.cpp

  Written by Moritz Bunkus <moritz@bunkus.org>

  Distributed under the GPL
  see the file COPYING for details
  or visit http://www.gnu.org/copyleft/gpl.html
*/

/*!
    \file
    \version \$Id$
    \brief ISO639 language definitions, lookup functions
    \author Moritz Bunkus <moritz@bunkus.org>
*/

#include <stdio.h>
#include <string.h>

#include "iso639.h"

iso639_language_t iso639_languages[] =
  {{"Abkhazian", "abk", "ab"},
   {"Achinese", "ace", NULL},
   {"Acoli", "ach", NULL},
   {"Adangme", "ada", NULL},
   {"Afar", "aar", "aa"},
   {"Afrihili", "afh", NULL},
   {"Afrikaans", "afr", "af"},
   {"Afro-Asiatic (Other)", "afa", NULL},
   {"Akan", "aka", "ak"},
   {"Akkadian", "akk", NULL},
   {"Albanian", "alb", "sq"},
   {NULL, "sqi", "sq"},
   {"Aleut", "ale", NULL},
   {"Algonquian languages", "alg", NULL},
   {"Altaic (Other)", "tut", NULL},
   {"Amharic", "amh", "am"},
   {"Apache languages", "apa", NULL},
   {"Arabic", "ara", "ar"},
   {"Aragonese", "arg", "an"},
   {"Aramaic", "arc", NULL},
   {"Arapaho", "arp", NULL},
   {"Araucanian", "arn", NULL},
   {"Arawak", "arw", NULL},
   {"Armenian", "arm", "hy"},
   {NULL, "hye", "hy"},
   {"Artificial (Other)", "art", NULL},
   {"Assamese", "asm", "as"},
   {"Asturian; Bable", "ast", NULL},
   {"Athapascan languages", "ath", NULL},
   {"Australian languages", "aus", NULL},
   {"Austronesian (Other)", "map", NULL},
   {"Avaric", "ava", "av"},
   {"Avestan", "ave", "ae"},
   {"Awadhi", "awa", NULL},
   {"Aymara", "aym", "ay"},
   {"Azerbaijani", "aze", "az"},
   {"Bable; Asturian", "ast", NULL},
   {"Balinese", "ban", NULL},
   {"Baltic (Other)", "bat", NULL},
   {"Baluchi", "bal", NULL},
   {"Bambara", "bam", "bm"},
   {"Bamileke languages", "bai", NULL},
   {"Banda", "bad", NULL},
   {"Bantu (Other)", "bnt", NULL},
   {"Basa", "bas", NULL},
   {"Bashkir", "bak", "ba"},
   {"Basque", "baq", "eu"},
   {NULL, "eus", "eu"},
   {"Batak (Indonesia)", "btk", NULL},
   {"Beja", "bej", NULL},
   {"Belarusian", "bel", "be"},
   {"Bemba", "bem", NULL},
   {"Bengali", "ben", "bn"},
   {"Berber (Other)", "ber", NULL},
   {"Bhojpuri", "bho", NULL},
   {"Bihari", "bih", "bh"},
   {"Bikol", "bik", NULL},
   {"Bini", "bin", NULL},
   {"Bislama", "bis", "bi"},
   {"Bokm�l, Norwegian; Norwegian Bokm�l", "nob", "nb"},
   {"Bosnian", "bos", "bs"},
   {"Braj", "bra", NULL},
   {"Breton", "bre", "br"},
   {"Buginese", "bug", NULL},
   {"Bulgarian", "bul", "bg"},
   {"Buriat", "bua", NULL},
   {"Burmese", "bur", "my"},
   {NULL, "mya", "my"},
   {"Caddo", "cad", NULL},
   {"Carib", "car", NULL},
   {"Castilian; Spanish", "spa", "es"},
   {"Catalan", "cat", "ca"},
   {"Caucasian (Other)", "cau", NULL},
   {"Cebuano", "ceb", NULL},
   {"Celtic (Other)", "cel", NULL},
   {"Central American Indian (Other)", "cai", NULL},
   {"Chagatai", "chg", NULL},
   {"Chamic languages", "cmc", NULL},
   {"Chamorro", "cha", "ch"},
   {"Chechen", "che", "ce"},
   {"Cherokee", "chr", NULL},
   {"Chewa; Chichewa; Nyanja", "nya", "ny"},
   {"Cheyenne", "chy", NULL},
   {"Chibcha", "chb", NULL},
   {"Chichewa; Chewa; Nyanja", "nya", "ny"},
   {"Chinese", "chi", "zh"},
   {NULL, "zho", "zh"},
   {"Chinook jargon", "chn", NULL},
   {"Chipewyan", "chp", NULL},
   {"Choctaw", "cho", NULL},
   {"Chuang; Zhuang", "zha", "za"},
   {"Church Slavic; Old Church Slavonic", "chu", "cu"},
   {"Old Church Slavonic; Old Slavonic; ", "chu", "cu"},
   {"Church Slavonic; Old Bulgarian; Church Slavic;", "chu", "cu"},
   {"Old Slavonic; Church Slavonic; Old Bulgarian;", "chu", "cu"},
   {"Church Slavic; Old Church Slavonic", "chu", "cu"},
   {"Chuukese", "chk", NULL},
   {"Chuvash", "chv", "cv"},
   {"Coptic", "cop", NULL},
   {"Cornish", "cor", "kw"},
   {"Corsican", "cos", "co"},
   {"Cree", "cre", "cr"},
   {"Creek", "mus", NULL},
   {"Creoles and pidgins (Other)", "crp", NULL},
   {"Creoles and pidgins,", "cpe", NULL},
   //   {"English-based (Other)", NULL, NULL},
   {"Creoles and pidgins,", "cpf", NULL},
   //   {"French-based (Other)", NULL, NULL},
   {"Creoles and pidgins,", "cpp", NULL},
   //   {"Portuguese-based (Other)", NULL, NULL},
   {"Croatian", "scr", "hr"},
   {NULL, "hrv", "hr"},
   {"Cushitic (Other)", "cus", NULL},
   {"Czech", "cze", "cs"},
   {NULL, "ces", "cs"},
   {"Dakota", "dak", NULL},
   {"Danish", "dan", "da"},
   {"Dargwa", "dar", NULL},
   {"Dayak", "day", NULL},
   {"Delaware", "del", NULL},
   {"Dinka", "din", NULL},
   {"Divehi", "div", "dv"},
   {"Dogri", "doi", NULL},
   {"Dogrib", "dgr", NULL},
   {"Dravidian (Other)", "dra", NULL},
   {"Duala", "dua", NULL},
   {"Dutch; Flemish", "dut", "nl"},
   {NULL, "nld", "nl"},
   {"Dutch, Middle (ca. 1050-1350)", "dum", NULL},
   {"Dyula", "dyu", NULL},
   {"Dzongkha", "dzo", "dz"},
   {"Efik", "efi", NULL},
   {"Egyptian (Ancient)", "egy", NULL},
   {"Ekajuk", "eka", NULL},
   {"Elamite", "elx", NULL},
   {"English", "eng", "en"},
   {"English, Middle (1100-1500)", "enm", NULL},
   {"English, Old (ca.450-1100)", "ang", NULL},
   {"Esperanto", "epo", "eo"},
   {"Estonian", "est", "et"},
   {"Ewe", "ewe", "ee"},
   {"Ewondo", "ewo", NULL},
   {"Fang", "fan", NULL},
   {"Fanti", "fat", NULL},
   {"Faroese", "fao", "fo"},
   {"Fijian", "fij", "fj"},
   {"Finnish", "fin", "fi"},
   {"Finno-Ugrian (Other)", "fiu", NULL},
   {"Flemish; Dutch", "dut", "nl"},
   {NULL, "nld", "nl"},
   {"Fon", "fon", NULL},
   {"French", "fre", "fr"},
   {NULL, "fra*", "fr"},
   {"French, Middle (ca.1400-1600)", "frm", NULL},
   {"French, Old (842-ca.1400)", "fro", NULL},
   {"Frisian", "fry", "fy"},
   {"Friulian", "fur", NULL},
   {"Fulah", "ful", "ff"},
   {"Ga", "gaa", NULL},
   {"Gaelic; Scottish Gaelic", "gla", "gd"},
   {"Gallegan", "glg", "gl"},
   {"Ganda", "lug", "lg"},
   {"Gayo", "gay", NULL},
   {"Gbaya", "gba", NULL},
   {"Geez", "gez", NULL},
   {"Georgian", "geo", "ka"},
   {NULL, "kat", "ka"},
   {"German", "ger", "de"},
   {NULL, "deu", "de"},
   {"German, Low; Saxon, Low; Low German; Low Saxon", "nds", NULL},
   {"German, Middle High (ca.1050-1500)", "gmh", NULL},
   {"German, Old High (ca.750-1050)", "goh", NULL},
   {"Germanic (Other)", "gem", NULL},
   {"Gikuyu; Kikuyu", "kik", "ki"},
   {"Gilbertese", "gil", NULL},
   {"Gondi", "gon", NULL},
   {"Gorontalo", "gor", NULL},
   {"Gothic", "got", NULL},
   {"Grebo", "grb", NULL},
   {"Greek, Ancient (to 1453)", "grc", NULL},
   {"Greek, Modern (1453-)", "gre", "el"},
   {NULL, "ell", "el"},
   {"Greenlandic; Kalaallisut", "kal", "kl"},
   {"Guarani", "grn", "gn"},
   {"Gujarati", "guj", "gu"},
   {"Gwich�in", "gwi", NULL},
   {"Haida", "hai", NULL},
   {"Hausa", "hau", "ha"},
   {"Hawaiian", "haw", NULL},
   {"Hebrew", "heb", "he"},
   {"Herero", "her", "hz"},
   {"Hiligaynon", "hil", NULL},
   {"Himachali", "him", NULL},
   {"Hindi", "hin", "hi"},
   {"Hiri Motu", "hmo", "ho"},
   {"Hittite", "hit", NULL},
   {"Hmong", "hmn", NULL},
   {"Hungarian", "hun", "hu"},
   {"Hupa", "hup", NULL},
   {"Iban", "iba", NULL},
   {"Icelandic", "ice", "is"},
   {NULL, "isl", "is"},
   {"Ido", "ido", "io"},
   {"Igbo", "ibo", "ig"},
   {"Ijo", "ijo", NULL},
   {"Iloko", "ilo", NULL},
   {"Inari Sami", "smn", NULL},
   {"Indic (Other)", "inc", NULL},
   {"Indo-European (Other)", "ine", NULL},
   {"Indonesian", "ind", "id"},
   {"Ingush", "inh", NULL},
   {"Interlingua (International", "ina", "ia"},
   //   {"Auxiliary Language Association)", NULL, NULL},
   {"Interlingue", "ile", "ie"},
   {"Inuktitut", "iku", "iu"},
   {"Inupiaq", "ipk", "ik"},
   {"Iranian (Other)", "ira", NULL},
   {"Irish", "gle", "ga"},
   {"Irish, Middle (900-1200)", "mga", NULL},
   {"Irish, Old (to 900)", "sga", NULL},
   {"Iroquoian languages", "iro", NULL},
   {"Italian", "ita", "it"},
   {"Japanese", "jpn", "ja"},
   {"Javanese", "jav", "jv"},
   {"Judeo-Arabic", "jrb", NULL},
   {"Judeo-Persian", "jpr", NULL},
   {"Kabardian", "kbd", NULL},
   {"Kabyle", "kab", NULL},
   {"Kachin", "kac", NULL},
   {"Kalaallisut; Greenlandic", "kal", "kl"},
   {"Kamba", "kam", NULL},
   {"Kannada", "kan", "kn"},
   {"Kanuri", "kau", "kr"},
   {"Kara-Kalpak", "kaa", NULL},
   {"Karen", "kar", NULL},
   {"Kashmiri", "kas", "ks"},
   {"Kawi", "kaw", NULL},
   {"Kazakh", "kaz", "kk"},
   {"Khasi", "kha", NULL},
   {"Khmer", "khm", "km"},
   {"Khoisan (Other)", "khi", NULL},
   {"Khotanese", "kho", NULL},
   {"Kikuyu; Gikuyu", "kik", "ki"},
   {"Kimbundu", "kmb", NULL},
   {"Kinyarwanda", "kin", "rw"},
   {"Kirghiz", "kir", "ky"},
   {"Komi", "kom", "kv"},
   {"Kongo", "kon", "kg"},
   {"Konkani", "kok", NULL},
   {"Korean", "kor", "ko"},
   {"Kosraean", "kos", NULL},
   {"Kpelle", "kpe", NULL},
   {"Kru", "kro", NULL},
   {"Kuanyama; Kwanyama", "kua", "kj"},
   {"Kumyk", "kum", NULL},
   {"Kurdish", "kur", "ku"},
   {"Kurukh", "kru", NULL},
   {"Kutenai", "kut", NULL},
   {"Kwanyama, Kuanyama", "kua", "kj"},
   {"Ladino", "lad", NULL},
   {"Lahnda", "lah", NULL},
   {"Lamba", "lam", NULL},
   {"Lao", "lao", "lo"},
   {"Latin", "lat", "la"},
   {"Latvian", "lav", "lv"},
   {"Letzeburgesch; Luxembourgish", "ltz", "lb"},
   {"Lezghian", "lez", NULL},
   {"Limburgan; Limburger; Limburgish", "lim", "li"},
   {"Limburger; Limburgan; Limburgish;", "lim", "li"},
   {"Limburgish; Limburger; Limburgan", "lim", "li"},
   {"Lingala", "lin", "ln"},
   {"Lithuanian", "lit", "lt"},
   {"Low German; Low Saxon; German, Low; Saxon, Low", "nds", NULL},
   {"Low Saxon; Low German; Saxon, Low; German, Low", "nds", NULL},
   {"Lozi", "loz", NULL},
   {"Luba-Katanga", "lub", "lu"},
   {"Luba-Lulua", "lua", NULL},
   {"Luiseno", "lui", NULL},
   {"Lule Sami", "smj", NULL},
   {"Lunda", "lun", NULL},
   {"Luo (Kenya and Tanzania)", "luo", NULL},
   {"Lushai", "lus", NULL},
   {"Luxembourgish; Letzeburgesch", "ltz", "lb"},
   {"Macedonian", "mac", "mk"},
   {NULL, "mkd", "mk"},
   {"Madurese", "mad", NULL},
   {"Magahi", "mag", NULL},
   {"Maithili", "mai", NULL},
   {"Makasar", "mak", NULL},
   {"Malagasy", "mlg", "mg"},
   {"Malay", "may", "ms"},
   {NULL, "msa", "ms"},
   {"Malayalam", "mal", "ml"},
   {"Maltese", "mlt", "mt"},
   {"Manchu", "mnc", NULL},
   {"Mandar", "mdr", NULL},
   {"Mandingo", "man", NULL},
   {"Manipuri", "mni", NULL},
   {"Manobo languages", "mno", NULL},
   {"Manx", "glv", "gv"},
   {"Maori", "mao", "mi"},
   {NULL, "mri", "mi"},
   {"Marathi", "mar", "mr"},
   {"Mari", "chm", NULL},
   {"Marshallese", "mah", "mh"},
   {"Marwari", "mwr", NULL},
   {"Masai", "mas", NULL},
   {"Mayan languages", "myn", NULL},
   {"Mende", "men", NULL},
   {"Micmac", "mic", NULL},
   {"Minangkabau", "min", NULL},
   {"Miscellaneous languages", "mis", NULL},
   {"Mohawk", "moh", NULL},
   {"Moldavian", "mol", "mo"},
   {"Mon-Khmer (Other)", "mkh", NULL},
   {"Mongo", "lol", NULL},
   {"Mongolian", "mon", "mn"},
   {"Mossi", "mos", NULL},
   {"Multiple languages", "mul", NULL},
   {"Munda languages", "mun", NULL},
   {"Nahuatl", "nah", NULL},
   {"Nauru", "nau", "na"},
   {"Navaho, Navajo", "nav", "nv"},
   {"Navajo; Navaho", "nav", "nv"},
   {"Ndebele, North", "nde", "nd"},
   {"Ndebele, South", "nbl", "nr"},
   {"Ndonga", "ndo", "ng"},
   {"Neapolitan", "nap", NULL},
   {"Nepali", "nep", "ne"},
   {"Newari", "new", NULL},
   {"Nias", "nia", NULL},
   {"Niger-Kordofanian (Other)", "nic", NULL},
   {"Nilo-Saharan (Other)", "ssa", NULL},
   {"Niuean", "niu", NULL},
   {"Norse, Old", "non", NULL},
   {"North American Indian (Other)", "nai", NULL},
   {"Northern Sami", "sme", "se"},
   {"North Ndebele", "nde", "nd"},
   {"Norwegian", "nor", "no"},
   {"Norwegian Bokm�l; Bokm�l, Norwegian", "nob", "nb"},
   {"Norwegian Nynorsk; Nynorsk, Norwegian", "nno", "nn"},
   {"Nubian languages", "nub", NULL},
   {"Nyamwezi", "nym", NULL},
   {"Nyanja; Chichewa; Chewa", "nya", "ny"},
   {"Nyankole", "nyn", NULL},
   {"Nynorsk, Norwegian; Norwegian Nynorsk", "nno", "nn"},
   {"Nyoro", "nyo", NULL},
   {"Nzima", "nzi", NULL},
   {"Occitan (post 1500},; Proven�al", "oci", "oc"},
   {"Ojibwa", "oji", "oj"},
   {"Old Bulgarian; Old Slavonic; Church Slavonic;", "chu", "cu"},
   {"Oriya", "ori", "or"},
   {"Oromo", "orm", "om"},
   {"Osage", "osa", NULL},
   {"Ossetian; Ossetic", "oss", "os"},
   {"Ossetic; Ossetian", "oss", "os"},
   {"Otomian languages", "oto", NULL},
   {"Pahlavi", "pal", NULL},
   {"Palauan", "pau", NULL},
   {"Pali", "pli", "pi"},
   {"Pampanga", "pam", NULL},
   {"Pangasinan", "pag", NULL},
   {"Panjabi", "pan", "pa"},
   {"Papiamento", "pap", NULL},
   {"Papuan (Other)", "paa", NULL},
   {"Persian", "per", "fa"},
   {NULL, "fas", "fa"},
   {"Persian, Old (ca.600-400 B.C.)", "peo", NULL},
   {"Philippine (Other)", "phi", NULL},
   {"Phoenician", "phn", NULL},
   {"Pohnpeian", "pon", NULL},
   {"Polish", "pol", "pl"},
   {"Portuguese", "por", "pt"},
   {"Prakrit languages", "pra", NULL},
   {"Proven�al; Occitan (post 1500)", "oci", "oc"},
   {"Proven�al, Old (to 1500)", "pro", NULL},
   {"Pushto", "pus", "ps"},
   {"Quechua", "que", "qu"},
   {"Raeto-Romance", "roh", "rm"},
   {"Rajasthani", "raj", NULL},
   {"Rapanui", "rap", NULL},
   {"Rarotongan", "rar", NULL},
   {"Reserved for local use", "qaa-qtz", NULL},
   {"Romance (Other)", "roa", NULL},
   {"Romanian", "rum", "ro"},
   {NULL, "ron", "ro"},
   {"Romany", "rom", NULL},
   {"Rundi", "run", "rn"},
   {"Russian", "rus", "ru"},
   {"Salishan languages", "sal", NULL},
   {"Samaritan Aramaic", "sam", NULL},
   {"Sami languages (Other)", "smi", NULL},
   {"Samoan", "smo", "sm"},
   {"Sandawe", "sad", NULL},
   {"Sango", "sag", "sg"},
   {"Sanskrit", "san", "sa"},
   {"Santali", "sat", NULL},
   {"Sardinian", "srd", "sc"},
   {"Sasak", "sas", NULL},
   {"Saxon, Low; German, Low; Low Saxon; Low German", "nds", NULL},
   {"Scots", "sco", NULL},
   {"Scottish Gaelic; Gaelic", "gla", "gd"},
   {"Selkup", "sel", NULL},
   {"Semitic (Other)", "sem", NULL},
   {"Serbian", "scc", "sr"},
   {NULL, "srp", "sr"},
   {"Serer", "srr", NULL},
   {"Shan", "shn", NULL},
   {"Shona", "sna", "sn"},
   {"Sichuan Yi", "iii", "ii"},
   {"Sidamo", "sid", NULL},
   {"Sign languages", "sgn", NULL},
   {"Siksika", "bla", NULL},
   {"Sindhi", "snd", "sd"},
   {"Sinhalese", "sin", "si"},
   {"Sino-Tibetan (Other)", "sit", NULL},
   {"Siouan languages", "sio", NULL},
   {"Skolt Sami", "sms", NULL},
   {"Slave (Athapascan)", "den", NULL},
   {"Slavic (Other)", "sla", NULL},
   {"Slovak", "slo", "sk"},
   {NULL, "slk", "sk"},
   {"Slovenian", "slv", "sl"},
   {"Sogdian", "sog", NULL},
   {"Somali", "som", "so"},
   {"Songhai", "son", NULL},
   {"Soninke", "snk", NULL},
   {"Sorbian languages", "wen", NULL},
   {"Sotho, Northern", "nso", NULL},
   {"Sotho, Southern", "sot", "st"},
   {"South American Indian (Other)", "sai", NULL},
   {"Southern Sami", "sma", NULL},
   {"South Ndebele", "nbl", "nr"},
   {"Spanish; Castilian", "spa", "es"},
   {"Sukuma", "suk", NULL},
   {"Sumerian", "sux", NULL},
   {"Sundanese", "sun", "su"},
   {"Susu", "sus", NULL},
   {"Swahili", "swa", "sw"},
   {"Swati", "ssw", "ss"},
   {"Swedish", "swe", "sv"},
   {"Syriac", "syr", NULL},
   {"Tagalog", "tgl", "tl"},
   {"Tahitian", "tah", "ty"},
   {"Tai (Other)", "tai", NULL},
   {"Tajik", "tgk", "tg"},
   {"Tamashek", "tmh", NULL},
   {"Tamil", "tam", "ta"},
   {"Tatar", "tat", "tt"},
   {"Telugu", "tel", "te"},
   {"Tereno", "ter", NULL},
   {"Tetum", "tet", NULL},
   {"Thai", "tha", "th"},
   {"Tibetan", "tib", "bo"},
   {NULL, "bod", "bo"},
   {"Tigre", "tig", NULL},
   {"Tigrinya", "tir", "ti"},
   {"Timne", "tem", NULL},
   {"Tiv", "tiv", NULL},
   {"Tlingit", "tli", NULL},
   {"Tok Pisin", "tpi", NULL},
   {"Tokelau", "tkl", NULL},
   {"Tonga (Nyasa)", "tog", NULL},
   {"Tonga (Tonga Islands)", "ton", "to"},
   {"Tsimshian", "tsi", NULL},
   {"Tsonga", "tso", "ts"},
   {"Tswana", "tsn", "tn"},
   {"Tumbuka", "tum", NULL},
   {"Tupi languages", "tup", NULL},
   {"Turkish", "tur", "tr"},
   {"Turkish, Ottoman (1500-1928)", "ota", NULL},
   {"Turkmen", "tuk", "tk"},
   {"Tuvalu", "tvl", NULL},
   {"Tuvinian", "tyv", NULL},
   {"Twi", "twi", "tw"},
   {"Ugaritic", "uga", NULL},
   {"Uighur", "uig", "ug"},
   {"Ukrainian", "ukr", "uk"},
   {"Umbundu", "umb", NULL},
   {"Undetermined", "und", NULL},
   {"Urdu", "urd", "ur"},
   {"Uzbek", "uzb", "uz"},
   {"Vai", "vai", NULL},
   {"Venda", "ven", "ve"},
   {"Vietnamese", "vie", "vi"},
   {"Volap�k", "vol", "vo"},
   {"Votic", "vot", NULL},
   {"Wakashan languages", "wak", NULL},
   {"Walamo", "wal", NULL},
   {"Walloon", "wln", "wa"},
   {"Waray", "war", NULL},
   {"Washo", "was", NULL},
   {"Welsh", "wel", "cy"},
   {NULL, "cym", "cy"},
   {"Wolof", "wol", "wo"},
   {"Xhosa", "xho", "xh"},
   {"Yakut", "sah", NULL},
   {"Yao", "yao", NULL},
   {"Yapese", "yap", NULL},
   {"Yiddish", "yid", "yi"},
   {"Yoruba", "yor", "yo"},
   {"Yupik languages", "ypk", NULL},
   {"Zande", "znd", NULL},
   {"Zapotec", "zap", NULL},
   {"Zenaga", "zen", NULL},
   {"Zhuang; Chuang", "zha", "za"},
   {"Zulu", "zul", "zu"},
   {"Zuni", "zun", NULL},
   {NULL, NULL, NULL}};

char *get_iso639_english_name(char *iso639_2_code) {
  int i;

  i = 0;
  while (iso639_languages[i].iso639_2_code != NULL) {
    if (!strcmp(iso639_languages[i].iso639_2_code, iso639_2_code))
      return iso639_languages[i].english_name;
    i++;
  }

  return NULL;
}

int is_valid_iso639_2_code(char *iso639_2_code) {
  int i;

  i = 0;
  while (iso639_languages[i].iso639_2_code != NULL) {
    if (!strcmp(iso639_languages[i].iso639_2_code, iso639_2_code))
      return i;
    i++;
  }

  return 0;
}

void list_iso639_languages() {
  int i;

  i = 0;
  fprintf(stdout, "                             English language name | "
          "ISO639-2 code | ISO639-1 code\n"
                  "---------------------------------------------------+-"
          "--------------+--------------\n");
  while (iso639_languages[i].iso639_2_code != NULL) {
    fprintf(stdout, "%50s | %13s | %13s\n",
            iso639_languages[i].english_name != NULL ?
            iso639_languages[i].english_name : "",
            iso639_languages[i].iso639_2_code,
            iso639_languages[i].iso639_1_code != NULL ?
            iso639_languages[i].iso639_1_code : "");
    i++;
  }
}
