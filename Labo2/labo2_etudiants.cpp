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
#include <fstream>

mutex mtx_list; // Mutex pour protéger l'accès à la liste partagée
mutex mtx_mat[MAX]; // Mutex pour protéger l'accès aux matériaux
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


void create(int i, craft_t *c){                                                                 // *** Début fonction create() ***************** //
    list<string>::iterator it = c->liste.begin();                                               // Création itérateur it pour faire une recherche.
    advance(it,1);                                                                              // On place l'itérateur à la seconde position de la file pour savoir la valeur.
  
  /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
  //AJOUTER VOTRE CODE ICI.
  /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

    // Recette de l'objet i : matériau i et matériau (i + 1) % 7
    int mat1 = i;
    int mat2 = (i + 1) % MAX;
    string req1 = c->name[mat1];
    string req2 = c->name[mat2];

    // Verrouillage du mutex de la liste partagée
    mtx_list.lock();

    if (c->liste.size() >= 2) {
        // On remet l'itérateur 'it' sur le 2e élément actuel de la liste (une fois le mutex verrouillé)
        it = c->liste.begin();
        advance(it, 1);

        // Si cet objet a déjà ses 10 exemplaires et que son matériau bloque la tête de la file,
        // il fait simplement tourner le 1er élément à la fin pour laisser la place aux autres !
        if (c->craft[i] >= 10) {
            if (c->liste.front() == req1 || c->liste.front() == req2) {
                string temp = c->liste.front();
                c->liste.pop_front();
                c->liste.push_back(temp);
            }
        }
        // Sinon (l'objet a moins de 10 exemplaires), il tente de fabriquer son objet :
        else {
            // CAS 1 : Le 1er élément est req1
            if (c->liste.front() == req1) {
                a[i] = c->liste.front();
                c->liste.pop_front();

                // Si le 2e élément (maintenant en tête, aussi pointé par *it) est req2
                if (c->liste.front() == req2) {
                    std::lock(mtx_mat[mat1], mtx_mat[mat2]);

                    b[i] = c->liste.front();
                    c->liste.pop_front();

                    cout << c->craft_name[i] << " en création." << endl;
                    fichier << c->craft_name[i] << " en création." << endl;

                    c->craft[i]++;
                    cout << "** " << c->craft_name[i] << "s crees: " << c->craft[i] << " **" << endl;
                    fichier << "** " << c->craft_name[i] << "s crees: " << c->craft[i] << " **" << endl;

                    this_thread::sleep_for(chrono::milliseconds(200));

                    // On remet b[i] puis a[i] pour casser les paires et permettre aux voisins de créer !
                    c->liste.push_back(b[i]);
                    c->liste.push_back(a[i]);

                    mtx_mat[mat1].unlock();
                    mtx_mat[mat2].unlock();
                } 
                else {
                    b[i] = "Waiting";
                    cout << a[i] << " is waiting" << endl;
                    cout << c->craft_name[i] << " en attente de " << req2 << "." << endl;
                    fichier << a[i] << " is waiting" << endl;
                    fichier << c->craft_name[i] << " en attente de " << req2 << "." << endl;

                    c->liste.push_back(a[i]);
                }
            }
            // CAS 2 : Le 1er élément est req2
            else if (c->liste.front() == req2) {
                a[i] = c->liste.front();
                c->liste.pop_front();

                // Si le 2e élément est req1
                if (c->liste.front() == req1) {
                    std::lock(mtx_mat[mat1], mtx_mat[mat2]);

                    b[i] = c->liste.front();
                    c->liste.pop_front();

                    cout << c->craft_name[i] << " en création." << endl;
                    fichier << c->craft_name[i] << " en création." << endl;

                    c->craft[i]++;
                    cout << "** " << c->craft_name[i] << "s crees: " << c->craft[i] << " **" << endl;
                    fichier << "** " << c->craft_name[i] << "s crees: " << c->craft[i] << " **" << endl;

                    this_thread::sleep_for(chrono::milliseconds(200));

                    // On remet b[i] puis a[i] pour mélanger la file
                    c->liste.push_back(b[i]);
                    c->liste.push_back(a[i]);

                    mtx_mat[mat1].unlock();
                    mtx_mat[mat2].unlock();
                } 
                else {
                    b[i] = "Waiting";
                    cout << a[i] << " is waiting" << endl;
                    cout << c->craft_name[i] << " en attente de " << req1 << "." << endl;
                    fichier << a[i] << " is waiting" << endl;
                    fichier << c->craft_name[i] << " en attente de " << req1 << "." << endl;

                    c->liste.push_back(a[i]);
                }
            }
        }
    }

    // Vérification si les 7 objets ont tous atteint 10 exemplaires
    bool tous_finis = true;
    for (int k = 0; k < MAX; k++) {
        if (c->craft[k] < 10) {
            tous_finis = false;
            break;
        }
    }
    if (tous_finis) {
        done = 1;
    }

    mtx_list.unlock();

                                                                                            ///////////////////////////////////////////////////
}                                                                                               // *** Fin fonction create() ******************* //                                                                                               // *** Fin fonction create() ******************* //                                                                                             // *** Fin fonction create() ******************* //                                                                                              // *** Fin fonction create() ******************* //


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
/**/    XDrawString(dis,win,gc,x,y+210,"Appuyer 'r' pour refresh page",25);                         /**/
/**/    XDrawString(dis,win,gc,x,y+220,"Appuyer 't' pour reset statut",29);                         /**/
/**/    XDrawString(dis,win,gc,x,y+230,"Appuyer 'q' pour quit",21);                                 /**/
/**/    XDrawString(dis,win,gc,x,y+250,"Recipes:",8);                                           /**/
/**/    XDrawString(dis,win,gc,x,y+260,"Argent + Or = Coupe",19);                            /**/
/**/    XDrawString(dis,win,gc,x,y+270,"Or + Bois = Porte",17);                             /**/
/**/    XDrawString(dis,win,gc,x,y+280,"Bois + Metal = Table",20);                            /**/
/**/    XDrawString(dis,win,gc,x,y+290,"Metal + Diamant = Bague",23);                             /**/
/**/    XDrawString(dis,win,gc,x,y+300,"Diamant + Fer = Chandelier",26);                         /**/
/**/    XDrawString(dis,win,gc,x,y+310,"Fer + Jade = Epee",17);                           /**/
/**/    XDrawString(dis,win,gc,x,y+320,"Jade + Argent = Bracelet",24);                           /**/
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
/**/    XDrawString(dis,win,gc,x,y,"Objet ",6);                                          /**/
/**/    XDrawString(dis,win,gc,x+72,y,kk.c_str(),1);                                            /**/
/**/    XDrawString(dis,win,gc,x+76,y," : ",3);                                                 /**/
/**/    XSetForeground(dis,gc,red);                                                             /**/
/**/    XDrawString(dis,win,gc,x+102,y,a[k].c_str(),a[k].size());a[k] = "Free";                 /**/
/**/    XDrawString(dis,win,gc,x+152,y,b[k].c_str(),b[k].size());b[k] = "Free";                 /**/
/**/    y = y + 10;                                                                             /**/
/**/  }                                                                                         /**/
/**/  if (done == 1){                                                                           /**/
/**/    XSetForeground(dis,gc,green);                                                           /**/
/**/    XDrawString(dis,win,gc,x,y+10,"Items completes!",15);                             /**/
/**/  }                                                                                         /**/
/**/  menu();                                                                                   /**/
/**/}                                                                                           /**/
/**/                                                                                            /**/
/**/unsigned long RGB(int r, int g, int b){                                                     /**/
/**/    return b + (g<<8) + (r<<16);                                                            /**/
/**/}                                                                                           /**/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/