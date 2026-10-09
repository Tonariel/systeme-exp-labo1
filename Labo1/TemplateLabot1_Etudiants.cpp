#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fstream>
#include <random>
#include <chrono>

using namespace std;

vector<string> historique_cmd;
vector<pid_t> historique_pid;

vector<string> commands = {"ls -l", "pwd", "ps", "ls -a", "ls", "cat TemplateLabot1_Etudiants.cpp"};

void afficherHistorique(string fileName = "historique.txt")
{
   int size = historique_cmd.size();    // on prend la taille du tableau
   int debut = size > 5 ? size - 5 : 0; // Si la taille est >  5 alors on prend la fin - 5, sinon on commence à 0

   ofstream fichier(fileName); // Créer et ouvre le fichier de l'historique

   for (size_t i = debut; i < size; i++)
   {
      cout << size - i << "\t" // on affiche dans la console l'historique
           << historique_cmd[i] << "\t"
           << historique_pid[i] << endl;

      fichier << size - i << "\t" // enregistre dans le fichier la sortie de la commande historique
              << historique_cmd[i] << "\t"
              << historique_pid[i] << endl;
   }

   fichier.close(); // ferme le fichier
}

int executerCommande(char *command);

void instance(int numInstance, int nbCommandes, int intervalHistorique)
{
   // permet de créer la randomisation
   random_device rd;
   mt19937 gen(rd());
   uniform_int_distribution<> distrib(0, commands.size() - 1); // choisi un chiffre entre 0 et la taille du tableau des commandes

   string fileName = "instance" + to_string(numInstance) + ".txt"; // créer le nom du fichier en fonction du numéro de l'instance

   ofstream fichier(fileName); // ouvre le fichier pour stocker les commandes exécuté

   auto start = chrono::steady_clock::now(); // prend le temps avant l'exécution de la boucle
   for (int i = 1; i <= nbCommandes; i++)
   {
      char command[128];

      strcpy(command, commands[distrib(gen)].c_str()); // transforme le string de la commande en char

      fichier << command << endl; // écris la commande dans le fichier
      executerCommande(command);  // exécute la commande

      if (i % intervalHistorique == 0) // Lance la commande historique en fonctione de l'interval
      {
         string fileName = "historique" + to_string(numInstance) + "_" + to_string(i) + ".txt";
         afficherHistorique(fileName);
      }
   }
   fichier.close();                        // ferme le fichier
   auto end = chrono::steady_clock::now(); // prend le temps après l'exécution de la boucle et de toutes les commandes

   auto duree_ms = chrono::duration_cast<chrono::milliseconds>(end - start).count(); // calcule la différence pour pouvoir obtenir le temps d'exécution en millisecondes

   cout << "Durée : " << duree_ms << "ms" << endl;
}

int executerCommande(char *command)
{
   string finalCommand = command;
   vector<char *> args;
   char *prog = strtok(command, " ");
   char *tmp = prog;
   bool background = false;

   while (tmp != NULL)
   {
      args.push_back(tmp);
      tmp = strtok(NULL, " ");
   }

   if (args.empty())
      return 0;

   if (strcmp(args[args.size() - 1], "&") == 0)
   {
      background = true;
      args.pop_back();
   }

   char **argv = new char *[args.size() + 1];

   for (int k = 0; k < args.size(); k++)
   {
      argv[k] = args[k];
   }

   argv[args.size()] = NULL;

   if ((strcmp(command, "exit") == 0) || strcmp(command, "stop") == 0)
   {
      return 1;
   }

   if (strcmp(command, "historique") == 0) // Si la commande = historique
   {
      afficherHistorique();
      return 0;
   }

   if (strcmp(command, "instance1") == 0)
   {
      instance(1, 100, 50); // Si la commande = instance1,
                            // alors on utilise la méthode instance pour créer l'instance avec l'exécution de 100 commandes, et l'exécution d'historique toutes les 50 commandes

      return 0;
   }

   if (strcmp(command, "instance2") == 0)
   {
      instance(2, 500, 100); // Pareil qu'au dessus, mais pour l'instance 2 donc l'exécution de 500 commandes, et l'exécution d'historique toutes les 100 commandes
      return 0;
   }

   if (prog[0] == '.')
   {
      std::system(args[0]);
   }
   else
   {
      pid_t kidpid = fork();

      if (kidpid < 0)
      {
         perror("Could not fork");
         return -1;
      }
      else if (kidpid == 0)
      {
         int result = execvp(prog, argv);
         if (result == -1)
         {
            perror("execvp");
            exit(1); // permet de terminer l'enfant si une erreur
         }
      }
      else
      {
         historique_pid.push_back(kidpid);
         historique_cmd.push_back(finalCommand);

         if (!background)
         {
            if (waitpid(kidpid, 0, 0) < 0)
            {
               return -1;
            }
         }
      }
   }
   return 0;
}

int main()
{
   while (true)
   {
      std::cout << "DetresVidil> ";
      char command[128];
      cin.getline(command, 128);

      if (strlen(command) != 0)
      {
         int result = executerCommande(command);

         if (result == 1)
         {
            break;
         }
      }
   }

   return 0;
}
