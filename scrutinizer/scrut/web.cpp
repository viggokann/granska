/* $Id: web.cpp,v 1.15 2006/09/09 03:55:06 viggo Exp $
 * Author: Johan Carlberger
 */

#include <fstream>
#include <sstream>
#include <errno.h>
#include "server_socket.h"
#include "scrutinizer.h"
#ifdef PROBCHECK  
#include "output.h"
#include "prob.h"
#endif // PROBCHECK

static Scrutinizer *xScrutinizer = NULL;
static const char *xRuleSet = NULL;
static char *granskaHome = "";
static char *granskaLogs = "";
static const int MAX_INFLECT_TEXT_LEN = 1024;
static const int MAX_TEXT_LEN = 1000000;
static const int MAX_URL_LEN = 1024;
static bool doLogging = false;
static std::ofstream logStream;

// statuskod tillbaks till scrut.php
// varje rad som skickas på socket till scut inleds med en fyrteckens label. Om fjärde tecknet är < så är allt efterföljande data fram till EOF, annars bara resten av raden.

enum StatusCode {
  SCRUT_NOT_YET_DETERMINED = 0,
  SCRUT_OK = 100,
  SCRUT_NO_INPUT = 200,
  SCRUT_BAD_URL = 201,
  SCRUT_TEXT_NOT_READ = 202,
  SCRUT_UP_CHECK = 300
};

// för att skapa loggfiler på formen YYMMDD
static inline int TodayYYMMDD() {
  time_t t;
  time(&t);
  const tm *m = localtime(&t);
  if (!m) return 0;
  int YYMMDD = (1900 + m->tm_year) * 10000;
  YYMMDD += (m->tm_mon + 1) * 100;
  YYMMDD += m->tm_mday;
  //  YYMMDD += m->tm_min; use to test switching
  return YYMMDD;
}

// för att testa webbgranska i terminal istf. browser
static bool GetTerminalTask(char *text, char *url) {
  *text = *url = '\0';
  for (char c = 'x';;) {
    std::cout << "(t)ext or (u)rl?: "; std::cin >> c;
    switch(c) {
    case 't':
      std::cout << "\nenter text: ";
      std::cin >> text;
      return true;
    case 'u':
      std::cout << "\nenter url: ";
      std::cin >> url;
      return true;
    default:
      ;
    }
  }
}

// skicka tillbaka en hårdkodad xmlfil, byt till att granska exempelmeningarna
static void SendXML(Socket &socket) {
  char fileName[1024];
  sprintf(fileName, "%s/exempel_litet.xml", granskaHome);
  std::ifstream in(fileName);
  if (in) {
    socket << "STAT 100\nTXT< ";
    socket << in.rdbuf();
    std::cerr << xRuleSet << ": xml-data sent." << std::endl;
  } else
    std::cerr << xRuleSet << ": cannot open " << fileName << std::endl;
}

static void ReadExamples(char *text) {
  char fileName[1024];
  sprintf(fileName, "%s/exempel_litet.txt", granskaHome);
  std::ifstream in(fileName);
  std::string temp, all_lines;
  while(in) {
    std::getline(in, temp);
    all_lines += temp;
    all_lines += " ";
  }
  strcpy(text, all_lines.c_str());
}

// hämta ett uppdrag från scrut:
static bool GetWebTask(ServerSocket *server, Socket &socket, char *text, char *url, char *inflect_text) {
  *inflect_text = *text = *url = '\0';
  std::cerr << xRuleSet << ": waiting for something to scrutinize...\n";
  // här kommer programmet att vänta tills nåt kommer:
  server->Accept(socket);
  // nu är ett uppdrag på väg
  std::cerr << xRuleSet << ": connected, reading task from socket\n";
  for (;;) {
    char buf[1024] = "";
    if (socket.peek() == EOF)
      return true;
    socket >> buf; // fel:.getline(buf, 5, ' ');
    if (!strcmp(buf, "TEXT")) {
      socket.get();
      socket.getline(text, MAX_TEXT_LEN-1);
    } else if (!strcmp(buf, "URL")) {
      socket.get();
      socket.getline(url, MAX_URL_LEN-1);
    } else if (!strcmp(buf, "INFLECT")) {
      socket.get();
      socket.getline(inflect_text, MAX_INFLECT_TEXT_LEN-1);
    } else if (!strcmp(buf, "ENDQ"))
      break;
    else if (!strcmp(buf, "CRSH")) {
      std::cerr << xRuleSet << ": got order to crash, exiting...\n";
      exit(1);
    } else if (!strcmp(buf, "LOOP")) {
      std::cerr << xRuleSet << ": got order to loop eternally, looping...\n";
      for (;;);
    } else if (!strcmp(buf, "XML")) {
      /*
      std::cerr << xRuleSet << ": got order to send xml-data...\n";
      SendXML(socket);
      return false;
      */
      ReadExamples(text);
      return true;
    } else {
      std::cerr << xRuleSet << ": unrecognized label: (" << buf << ')' << std::endl;
      while (socket.peek() != '\n' && socket.peek() != EOF)
	socket.get();
    }
  }
  return true;
}

// skriv resultat till terminal vid terminalmode:
static void PutTerminalResult(char *text, char *url, char *reply) {
  std::cout << "Granska says about ";
  if (*text)
    std::cout << "text " << text;
  else
    std::cout << "url " << url;
  std::cout << ":\n" << reply << std::endl;
}

// hämta innehållet i URLen med wget och spar i buf:
static bool GetURLContent(const char *url, char *buf, int maxLen) {
  *buf = '\0';
  char command[2000];
  // 10 sekunder väntan, två försök:
  sprintf(command, "wget -T 10 -t 2 -o /tmp/wgetlog -O - %s", url);
  FILE *fin = popen(command, "r");
  bool result = true;
  if (!fin) {
    std::cerr << xRuleSet << ": cannot wget url " << url << std::endl;
    result = false;
  }
  fread(buf, maxLen, 1, fin);
  pclose(fin);
  if (!*buf) {
    std::cerr << xRuleSet << ": nothing read from url " << url << std::endl;
    std::ifstream wgetlog("/tmp/wgetlog");
    if (wgetlog)
      wgetlog.read(buf, maxLen);
    result = false;
  }
  if (!result && !*buf) 
    strcpy(buf, "Granska kan inte hämta webbsidan");
  return result;
}

// körs lite då och då för att kolla om det blitt en ny dag:
static void MaybeSwitchLogStream() {
  static int todayYYMMDD = -1;
  const int yymmdd = TodayYYMMDD();
  if (yymmdd != todayYYMMDD) {
    //    std::cerr << xRuleSet << ": switch log " << yymmdd << std::endl;
    if (logStream) logStream.close();
    todayYYMMDD = yymmdd;
    char fileName[1024];
    //jonas    sprintf(fileName, "%s/logs/%s%i", granskaHome, xRuleSet, yymmdd);
    sprintf(fileName, "%s/%s%i", granskaLogs, xRuleSet, yymmdd);
    logStream.clear(); // fail bit set sometimes, but since we open a file anew, clear all bits.
    // här borde man öppna i appand-mode, annars kommer det försvinna loggrader:
    logStream.open(fileName, std::ofstream::out | std::ofstream::app);
    if (logStream.is_open())
      doLogging = true;
    else {
      std::cerr << xRuleSet << ": cannot append to logg-file " << fileName 
		<< ", " << strerror(errno) << std::endl;
      std::cerr << "ignoring error\n";
      doLogging = true;
      /*
	logStream.open(fileName);
	if (!logStream) {
	std::cerr << xRuleSet << ": cannot open logg-file " << fileName 
		  << ", " << strerror(errno) << std::endl;
	doLogging = false;
      }
      */
    }
  }
}

static void LogInput(const char *text, const char *url) {
  if (!doLogging) return;
  if (*text) logStream << "TEXT" << tab << text << std::endl;
  else if (*url) logStream << "URL" << tab << url << std::endl;
}

static void LogOutput(const char *text, const char *url, const char *reply) {
  if (!doLogging) return;
  logStream << "REPLY" << tab << reply << std::endl;
}

// den här granskar och svarar och borde därför döpas om:
void ScrutinizeText(Socket &socket, char *text) {
  const int N = 40;
  char ugly = text[N];
  text[N] = 0; // only print first N letters
  std::cerr << xRuleSet << ": got text for Granska to read: (" << text << ")...\n";
  text[N] = ugly;
  const Text *t = xScrutinizer->ReadTextFromString(text);
  if (!t) {
    std::cerr << xRuleSet << ": Granska did not read the text.\n";
    socket << "STAT " << SCRUT_TEXT_NOT_READ << std::endl
	   << "REPL Mysko. Granska kunde inte läsa texten." << std::endl;
    return;
  }
  std::cerr << xRuleSet << ": Granska read the text, now scrutinizing...\n";
  socket << "STAT " << SCRUT_OK << std::endl
	 << "REPL Granska har granskat texten." << std::endl;
  socket << "TXT< ";
#ifdef PROBCHECK
  Prob::Output &out = Prob::output(socket);
  out.init();
#endif // PROBCHECK
  int nErrors = 0;
  xScrutinizer->Scrutinize(&nErrors);
  xScrutinizer->PrintResult();
#ifdef PROBCHECK
  out.exit();
#endif // PROBCHECK
  socket << std::endl;
  std::cerr << xRuleSet << ": Scrutinizing done, reply sent.\n";

#if 0
  // this prints the reply to the logfile, very verbose format...
  logStream << "REPLY\t";
  Prob::Output &log_out = Prob::output(logStream);
  log_out.init();
  xScrutinizer->PrintResult();
  log_out.exit();
  logStream << std::endl;
#endif
}

// en socket "socket" skapas vid varje anrop, när socket deletas så anropas close() och EOF skickas.
static bool Loop(ServerSocket *server) {
  if(doLogging)
    MaybeSwitchLogStream();
  char text[MAX_TEXT_LEN] = "";
  char url[MAX_URL_LEN] = "";
  char inflect_text[MAX_INFLECT_TEXT_LEN] = "";
  //StatusCode status = SCRUT_NOT_YET_DETERMINED;
  Socket socket;
  if (server) {
    if (!GetWebTask(server, socket, text, url, inflect_text))
      return true;
    if (!strcmp(text, "areuup4711")) {
      std::cerr << xRuleSet << ": got areuup, replying...\n";
      socket << "STAT " << SCRUT_UP_CHECK << std::endl
	     << "REPL yesiamup" << std::endl;
      return true;
    }
    LogInput(text, url);
  } else
    GetTerminalTask(text, url);
  if (*text && !strcmp(text, "exit4711")) {
    std::cerr << xRuleSet << ": got exit4711, terminating...\n";
    return false;
  }
  char reply[1024] = "inget svar påhittat än";
  if (*text)
    ScrutinizeText(socket, text);
  else if (*url) {
    text[0] = 0;
    std::ostringstream command_buf;
    command_buf << "lynx -dump -nolist " << url << " | tr '[:space:]' ' ' | tr -s ' ' | sed 's/\\[INLINE\\]//g'";
    FILE *lynx_pipe = popen(command_buf.str().c_str(), "r");
    fread(text, MAX_TEXT_LEN, 1, lynx_pipe);
    ScrutinizeText(socket, text);
    /*
    if (GetURLContent(url, text, MAX_TEXT_LEN)) {
      std::cerr << xRuleSet << ": URL contents fetched.\n";
      status = SCRUT_OK;
      // nu har bara innehållet hämtats. här ska man köra html2txt och sen granska och skicka tibaks svaret.
    } else {
      socket << "STAT " << SCRUT_BAD_URL << std::endl
	     << "REPL Webbsidan kunde inte hämtas:" << std::endl
	     << "TXT< " << text << std::endl; 
    }
    */
  } else if(*inflect_text) {
    int n = strlen(inflect_text) - 1;
    while(inflect_text[n] == '\r' ||
          inflect_text[n] == '\n') {
      inflect_text[n] = 0;
      n--;
    }
    while(n >= 0) {
      if(inflect_text[n] >= 'A' 
	 && inflect_text[n] <= 'Z')
	inflect_text[n] = inflect_text[n] - 'A' + 'a';
//       else if(inflect_text[n] <= 'z')
// 	; // skip common non-upper case characters 
      else if(inflect_text[n] == 'Å')
	inflect_text[n] = 'å';
      else if(inflect_text[n] == 'Ä')
	inflect_text[n] = 'ä';
      else if(inflect_text[n] == 'Ö')
	inflect_text[n] = 'ö';
      n--;
    }
    xScrutinizer->Words().ServerAnalyzeWordAndPrintInflections(socket, inflect_text);    
  } else {
    // varken text eller URL kom in, alltså fel:
    std::cerr << xRuleSet << ": no input.\n";
    socket << "STAT " << SCRUT_NO_INPUT << std::endl
	   << "REPL Fick inget att granska." << std::endl;
  }
  if (server) {
    //LogOutput(text, url, reply); // jsh, this line does nothing useful right now ("reply" never set)
    //PutWebResult(socket, status, text, url, reply);
  } else
    PutTerminalResult(text, url, reply);
  return true;
}

// skriv data till scrut så att den kan prata med mig:
// Filen variables.inc och andra filer som php-programmen läser
// måste kunna läsas av andra. 
static void MakePhpStuff(int webPort, const char *mode,
			 const char *desc) {
  char fileName[1024];
  sprintf(fileName, "%s/rulesets/%s/variables.inc",
	  granskaHome, xRuleSet);
  std::ofstream out(fileName);
  if (out) {
    out << "<?php "
	<< "$PORT = " << webPort << ";\n"
	<< "$HOST = 'localhost';\n" //" << getenv("HOST") << "';\n"
	<< "$MODE = '" << mode << "';\n"
	<< "$DESC = '" << desc << "';\n"   
	<< "?>\n";
  }
}

bool LoadRules() {
  char fileName[2048];
  sprintf(fileName, "%s/rulesets/%s/therules", granskaHome, xRuleSet);
  std::cerr << xRuleSet << ": loading " << fileName << std::endl;
  return xScrutinizer->Load(NULL, fileName);
}

int WebScrutinize(Scrutinizer *scrut, int argc, char **argv) {
  xScrutinizer = scrut;

  granskaHome = getenv("GRANSKA_HOME");
  if (!granskaHome) {
    std::cerr << "env-var GRANSKA_HOME not defined, quitting...\n";
    return 1;
  }
  granskaLogs = getenv("GRANSKA_LOGS");
  if(!granskaLogs)
    granskaLogs = granskaHome;
  ServerSocket *server = NULL;
  if (argc != 4) {
    std::cerr << "Usage: " << " ruleset port mode description\n"
	 << "Your " << argc << " arguments were\n";
    for (int i=0; i<argc; i++)
      std::cerr << argv[i] << std::endl;
    exit(1);
  }
  xRuleSet = argv[0];
  const int webPort = atoi(argv[1]);
  const char *mode = argv[2];
  const char *desc = argv[3];
  if (strcmp(mode, "public") && strcmp(mode, "private") &&
      strcmp(mode, "test")) {
    std::cerr << xRuleSet << ": bad mode " << mode << ", quitting";
    return 1;
  }
  if (webPort > 99) {
    std::cerr << xRuleSet << ": running web-" << argv[0] << " with " << xRuleSet
	 << " on port " << webPort << "..." << std::endl;
    if (!LoadRules()) {
      std::cerr << xRuleSet << ": cannot load rules " << xRuleSet << ", bailing out\n";
      return 1;
    }
#ifdef PROBCHECK    // jb: added probcheck support 2003-02-21
    Prob::load(xScrutinizer->Tags());
#endif // PROBCHECK
    server = new ServerSocket(webPort);
    if (!server || !server->IsOK()) {
      // porten antagligen upptagen, hitta på ett annat portnummer:
      std::cerr << xRuleSet << ": cannot create ServerSocket for port " << webPort << std::endl;
      return 1;
    }
    MakePhpStuff(webPort, mode, desc);
  }

  std::cerr << xRuleSet << ": Everything seems OK, entered "
       << (server ? "web" : "terminal")
       << "-task-loop" << std::endl;

  // ta emot texter och svara:
  while(Loop(server));
  return 0;
}

