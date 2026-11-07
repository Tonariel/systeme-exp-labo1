/* NE PAS TOUCHER */
#include <iostream>
#include <vector>
#include <list>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <chrono>
#include <string>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
using namespace std;
#define MAX 7                     // On définie le nombre de thread qu'on veut utiliser
bool done = 0;                    // Variable qui sert à savoir si on a terminé de créer tous les objets
string a[MAX] = {"Free", "Free", "Free", "Free", "Free", "Free", "Free"}; // Tableaux tampons pour recevoir les matériaux pour 
string b[MAX] = {"Free", "Free", "Free", "Free", "Free", "Free", "Free"}; // la fabrication et l'affichage.

/////////////////////////////////////////////////////////////
//AJOUTER VOS INCLUDE ET AUTRES ICI!!!!!!!!!!!!!!!!!!!!!!!!//

#include <mutex>
#include <algorithm>
#include <fstream>

string craft_a[MAX]{"Or", "Bois", "Metal", "Diamant", "Fer", "Jade", "Jade"};
string craft_b[MAX]{"Argent", "Or", "Bois", "Metal", "Diamant", "Fer", "Argent"};

std::mutex m;
int prio = 3;
int passages = 0;
ofstream fichier("sortieCreation.txt");
/////////////////////////////////////////////////////////////



/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! NE PAS TOUCHER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/**/typedef struct {                                                                                                                           /**/
/**/    string name[MAX]{"Argent", "Or", "Bois", "Metal", "Diamant", "Fer", "Jade"};        // Nom des matériaux                              /**/
/**/    list<string> liste{"Argent", "Or", "Bois", "Metal", "Diamant", "Fer", "Jade"};      // Nom des matériaux dans une liste               /**/
/**/    string craft_name[MAX]{"Coupe", "Porte", "Table", "Bague", "Chandelier", "Epee", "Bracelet"};   // Nom des objets fabriqués                /**/
/**/    int craft[MAX]{0,0,0,0,0,0,0};                                 // /**/
/**/} craft_t;                                                                                                                                 /**/
/**/                                                                                                                                           /**/
/**/Display *dis;                                                           // Sert pour l'affichage du GUI                                    /**/
/**/int screen;                                                             // Sert pour l'affichage du GUI                                    /**/
/**/Window win;                                                             // Sert à créer la fenêtre pour le GUI                             /**/
/**/GC gc;                                                                  // graphic context                                                 /**/
/**/unsigned long black, white, red, blue, green;                           // Sert pour les couleurs dans le GUI                              /**/
/**/int x, y;                                                               // Sert pour les coordonnées                                       /**/
/**/                                                                                                                                           /**/
/**/void init();                                                            // Fonction pour initialiser le GUI                                /**/
/**/void close();                                                           // Fonction pour fermer le GUI et le programme                     /**/
/**/void refresh();                                                         // Fonction pour rafraîchir l'affichage graphique                  /**/
/**/void menu();                                                            // Fonction pour afficher le menu dans le GUI                      /**/
/**/void afficher(craft_t *c);                                              // Fonction pour afficher les ressources et les objets fabriqués   /**/
/**/unsigned long RGB(int r, int g, int b);                                 // Fonction pour convertir les nombres en couleurs RGB             /**/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/


void create(int i, craft_t *c) {
  string elem_1 = craft_a[i];
  string elem_2 = craft_b[i];

  while (true) {
      m.lock();

      if (i == prio || passages != 0) {
        auto it1 = find(c->liste.begin(), c->liste.end(), elem_1);
        auto it2 = find(c->liste.begin(), c->liste.end(), elem_2);

        if (it1 != c->liste.end() && it2 != c->liste.end()) {

            c->liste.erase(it1);
            c->liste.erase(it2);

            a[i] = elem_1;
            b[i] = elem_2;

            cout << c->craft_name[i] << " en création" << endl;
            fichier << c->craft_name[i] << " en création" << endl;
            
            m.unlock();
            this_thread::sleep_for(chrono::milliseconds(200));
            m.lock();
            c->liste.push_back(elem_1);
            c->liste.push_back(elem_2);
            c->craft[i]++;


            cout << "** " << c->craft_name[i] << "s crees : " << c->craft[i] << " **" << endl;
            fichier << "** " << c->craft_name[i] << "s crees : " << c->craft[i] << " **" << endl;
        } else {
            a[i] = (it1 != c->liste.end()) ? elem_1 : "Waiting";
            b[i] = (it2 != c->liste.end()) ? elem_2 : "Waiting";

            if (it1 == c->liste.end()) {
              cout << c->craft_name[i] << " en attente de " << elem_1 << endl;
              fichier << c->craft_name[i] << " en attente de " << elem_1 << endl;
            }

            if (it2 == c->liste.end()) {
              cout << c->craft_name[i] << " en attente de " << elem_2 << endl;
              fichier << c->craft_name[i] << " en attente de " << elem_2 << endl;
            }
        }

        passages++;
        if (passages == MAX) {
          passages = 0;

          prio++;

          if (prio == MAX) prio = 0;
        }

        m.unlock();
        break;
    }

    m.unlock();
  }

  m.lock();

  bool termine = true;

  for (int j = 0; j < MAX; j++) {
    if (c->craft[j] < 10) {
        termine = false;
        break;
    }
  }

  if (termine) {
    done = true;
  }

  m.unlock();
}
                                                                                              // *** Fin fonction create() ******************* //

/*!!!!!!!!!! NE PAS TOUCHER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
void start(craft_t *c){                                                                         // *** Début fonction start() ****************** //
  vector<thread> threads(MAX);                                                                  // On crée un vecteur de threads.
                                                                                                // 
  for (int i = 0; i < MAX; ++i) {                                                               ///////////////////////////////////////////////////
     threads[i] = thread([i, c] { create(i, c);});                                              // La première boucle crée et utilise un thread
  }                                                                                             // et d'appeler la fonction create().                                             
  for (auto& t : threads) {                                                                     // La deuxième boucle permet de joindre les threads
    t.join();                                                                                   // au fur et à mesure.
  }                                                                                             ///////////////////////////////////////////////////
  cout << "_____________________________________________"<< endl;                               // Ligne permettant de séparer dans le CLI.
  fichier << "_____________________________________________"<< endl;                            // Ligne permettant de séparer dans le fichier.
}                                                                                               // *** Fin fonction start() ******************** //


/*!!!!!!!!!! NE PAS TOUCHER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
int main() {                                                                                    // *** Début fonction main() ******************* //
  craft_t craft;                                                                                // Permet de créer l'objet craft, qui va stocker nos informations tout au long du programme.    
  x = 8; y = 10;                                                                                // Permet de configurer les coordonnées de bases pour le GUI.
  init();                                                                                       // Initilialise le GUI.
  XEvent event;                                                                                 // Permet de savoir le type d'evenement.
  char text[1];                                                                                 // Permet d'entreposer la valeur du clavier.
                                                                                                //
  while (1){                                                                                    ///////////////////////////////////////////////////
    afficher(&craft);                                                                           // On affiche dans le GUI.
    y = 10;                                                                                     // On remet le y à 10 pour les prochaines affichages.
    XNextEvent(dis, &event);                                                                    // Se prépare à recevoir l'evenement.
    if(event.type==KeyPress && XLookupString(&event.xkey, text, 255, NULL,0)==1){               // Si il y a eu un evenement (en appuyant sur le clavier):
        if(text[0]=='s'){                                                                       // Option "s":
          if (done != 1){                                                                       // Tant que la fabrication n'est pas terminé,
            start(&craft);                                                                      // On commence un cycle de fabrication d'objet.
          }                                                                                     //
        }                                                                                       // _____________________________________________ //
        if(text[0]=='r'){                                                                       // Option "r":
          if (done != 1){                                                                       // Si la frabrication d'objet n'est pas terminé,
            afficher(&craft);                                                                   // On "rafraîchit" l'affichage.
          }                                                                                     //
        }                                                                                       // _____________________________________________ //
        if(text[0]=='t'){                                                                       // Option "t":
          if (done != 1){                                                                       // Si la frabrication d'objet n'est pas terminé,
            for (int i = 0; i < MAX; ++i) {                                                     // On "reset" la file en remettant chaque ressources,
              a[i] = b[i] = "Free";                                                             // dans un cas où c'est buggé.
              craft.liste ={"Argent", "Or", "Bois", "Metal", "Diamant", "Fer", "Jade"};          //
            }                                                                                   //
          }                                                                                     //
        }                                                                                       // _____________________________________________ //
        if(text[0]=='q'){                                                                       // Option "q":
          close();                                                                              // On ferme le programme.
        }                                                                                       //
    }                                                                                           //
  }                                                                                             ///////////////////////////////////////////////////
                                                                                                //
  return 0;                                                                                     //
}                                                                                               // *** Fin fonction main() ********************* //

/*!!!!!!!!!! NE PAS TOUCHER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/**/void init(){                                                                                /**/
/**/    x = 100; y = 100;                                                                       /**/
/**/    dis = XOpenDisplay((char *)0);                                                          /**/
/**/    screen = DefaultScreen(dis);                                                            /**/
/**/    black = BlackPixel(dis, screen);                                                        /**/
/**/    white = WhitePixel(dis, screen);                                                        /**/
/**/    red = RGB(255,0,0);                                                                     /**/
/**/    green = RGB(0,255,0);                                                                   /**/
/**/    blue = RGB(0,0,255);                                                                    /**/
/**/    win = XCreateSimpleWindow(dis, DefaultRootWindow(dis),0,0,350,350,5, black, white);     /**/
/**/    XSetStandardProperties(dis,win,"Lab 2 - 8INF342",NULL, None, NULL, 0, NULL);            /**/
/**/    XSelectInput(dis, win, ExposureMask | ButtonPressMask | KeyPressMask);                  /**/
/**/    gc = XCreateGC(dis, win, 0, 0);                                                         /**/
/**/    XSetBackground(dis,gc,white);                                                           /**/
/**/    XSetForeground(dis,gc,black);                                                           /**/
/**/    XClearWindow(dis,win);                                                                  /**/
/**/    XMapRaised(dis, win);                                                                   /**/
/**/}                                                                                           /**/
/**/                                                                                            /**/
/**/void close(){                                                                               /**/
/**/    XFreeGC(dis, gc);                                                                       /**/
/**/    XDestroyWindow(dis,win);                                                                /**/
/**/    XCloseDisplay(dis);                                                                     /**/
/**/    exit(0);                                                                                /**/
/**/}                                                                                           /**/
/**/                                                                                            /**/
/**/void refresh(){                                                                             /**/
/**/    XClearWindow(dis, win);                                                                 /**/
/**/    x = 8;                                                                                  /**/
/**/    y = 10;                                                                                 /**/
/**/}                                                                                           /**/
/**/                                                                                            /**/
/**/void menu(){                                                                                /**/
/**/    x = 8;                                                                                  /**/
/**/    y = 10;                                                                                 /**/
/**/    XSetForeground(dis,gc,BlackPixel(dis, screen));                                         /**/
/**/    XDrawString(dis,win,gc,x,y+200,"Appuyer sur 's' pour cycle",26);                        /**/
/**/    XDrawString(dis,win,gc,x,y+210,"Appuyer 'r' pour refresh page",25);                     /**/
/**/    XDrawString(dis,win,gc,x,y+220,"Appuyer 't' pour reset statut",29);                     /**/
/**/    XDrawString(dis,win,gc,x,y+230,"Appuyer 'q' pour quit",21);                             /**/
/**/    XDrawString(dis,win,gc,x,y+250,"Recipes:",8);                                           /**/
/**/    XDrawString(dis,win,gc,x,y+260,"Argent + Or = Coupe",19);                               /**/
/**/    XDrawString(dis,win,gc,x,y+270,"Or + Bois = Porte",17);                                 /**/
/**/    XDrawString(dis,win,gc,x,y+280,"Bois + Metal = Table",20);                              /**/
/**/    XDrawString(dis,win,gc,x,y+290,"Metal + Diamant = Bague",23);                           /**/
/**/    XDrawString(dis,win,gc,x,y+300,"Diamant + Fer = Chandelier",26);                        /**/
/**/    XDrawString(dis,win,gc,x,y+310,"Fer + Jade = Epee",17);                                 /**/
/**/    XDrawString(dis,win,gc,x,y+320,"Jade + Argent = Bracelet",24);                          /**/
/**/}                                                                                           /**/
/**/                                                                                            /**/
/**/void afficher(craft_t *c){                                                                  /**/
/**/  y = 10;                                                                                   /**/
/**/  XClearWindow(dis, win);                                                                   /**/
/**/  for(int k = 0; k <7; k++){                                                                /**/
/**/    string nbr = to_string(c->craft[k]);                                                    /**/
/**/    XSetForeground(dis,gc,BlackPixel(dis, screen));                                         /**/
/**/    XDrawString(dis,win,gc,x,y,c->craft_name[k].c_str(),c->craft_name[k].size());           /**/
/**/    XDrawString(dis,win,gc,x+42,y," : ",3);                                                 /**/
/**/    XSetForeground(dis,gc,blue);                                                            /**/
/**/    XDrawString(dis,win,gc,x+60,y,nbr.c_str(),nbr.size());                                  /**/
/**/    y = y + 10;                                                                             /**/
/**/  }                                                                                         /**/
/**/  y = y +30;                                                                                /**/
/**/  for(int k = 0; k <7; k++){                                                                /**/
/**/    string kk = to_string(k+1);                                                             /**/
/**/    XSetForeground(dis,gc,BlackPixel(dis, screen));                                         /**/
/**/    XDrawString(dis,win,gc,x,y,"Objet ",6);                                                 /**/
/**/    XDrawString(dis,win,gc,x+72,y,kk.c_str(),1);                                            /**/
/**/    XDrawString(dis,win,gc,x+76,y," : ",3);                                                 /**/
/**/    XSetForeground(dis,gc,red);                                                             /**/
/**/    XDrawString(dis,win,gc,x+102,y,a[k].c_str(),a[k].size());a[k] = "Free";                 /**/
/**/    XDrawString(dis,win,gc,x+152,y,b[k].c_str(),b[k].size());b[k] = "Free";                 /**/
/**/    y = y + 10;                                                                             /**/
/**/  }                                                                                         /**/
/**/  if (done == 1){                                                                           /**/
/**/    XSetForeground(dis,gc,green);                                                           /**/
/**/    XDrawString(dis,win,gc,x,y+10,"Items completes!",15);                                   /**/
/**/  }                                                                                         /**/
/**/  menu();                                                                                   /**/
/**/}                                                                                           /**/
/**/                                                                                            /**/
/**/unsigned long RGB(int r, int g, int b){                                                     /**/
/**/    return b + (g<<8) + (r<<16);                                                            /**/
/**/}                                                                                           /**/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/