#include <iostream>       // basic input output streams
#include <fstream>        // input output file stream class
#include <cmath>          // librerie mathematique de base
#include <iomanip>        // input output manipulators
#include <valarray>       // valarray functions
#include "ConfigFile.h" // Il contient les methodes pour lire inputs et ecrire outputs 
                          // Fichier .tpp car inclut fonctions template
#include <numeric>

#ifdef SDL_VISUALIZE
#include "SDL_render.hpp"
#endif

using namespace std; // ouvrir un namespace avec la librerie c++ de base

/* TODO
 *  1. Implementer les schemas d'Euler, avec param alpha
 *  2. Positions de la Terre et la lune.   OK
 *  3. Vitesse de rotation   OK
 *  4. Calcul de la position initiale (vitesse initiale)  OK
 *  5. Calcul de l'energie mecanique   OK
 */

 
#define SQUARE(x) ((x) * (x))

double sq(double x){
  return x * x;
}

void print_vect(const valarray<double>& arr){
  cout << "vx: " << arr[0] << " vy: " << arr[1] << " x: " << arr[2] << " y: " << arr[3] << endl;
}
double calc_norm(const valarray<double>& arr){
  double sum_norm_sqr = 0;
  for(size_t i = 0; i < arr.size(); ++i){
    //cout << i << "th :" << arr[i] << endl;
    sum_norm_sqr += SQUARE(arr[i]);
  }
  return sqrt(sum_norm_sqr);
}
/* La class Engine est le moteur principale de ce code. Il contient 
   les methodes de base pour lire / initialiser les inputs, 
   preparer les outputs et calculer les donnees necessaires
*/
class Engine
{
private:

// EngineEuler specific members
  unsigned int maxit; // nombre maximale d iterations
  double tol;         // tolerance methode iterative
  double alpha;       // parametre pour le scheme d'Euler

// définition des variables
double tfin;         // Temps final
unsigned int nsteps; // Nombre de pas de temps
double ml;           // Masse de la Lune
double mt;           // Masse de la Terre
double ms;
double dist;         // Distance Terre-Lune
double Om;           // Vitesse de rotation du repère
double G_grav;       // Constante gravitationnelle
double xt;           // Position de la Terre
double xl;           // Position de la Lune
double dist_s_t;     // Distance satellite-Terre
double dist_s_l;     // Distance satellite-Lune

  // (vx vy x y)
  valarray<double> y0 = std::valarray<double>(0.e0, 4); // Correctly initialized
  valarray<double> y  = std::valarray<double>(0.e0, 4); // Correctly initialized

  double t,dt;  // Temps courant pas de temps

  unsigned int sampling;  // Nombre de pas de temps entre chaque ecriture des diagnostics
  unsigned int last;       // Nombre de pas de temps depuis la derniere ecriture des diagnostics
  ofstream *outputFile;    // Pointeur vers le fichier de sortie

  /* Calculer et ecrire les diagnostics dans un fichier
     inputs:
     write: (bool) ecriture de tous les sampling si faux
  */  

  double calcDist(double x_other){
    //cout << y[2] << ' ' << y[3] << endl;
    return sqrt(sq(y[2] - x_other) + sq(y[3]));
  }

  double calculateEnergy(){
    double vx_sat = y[0], vy_sat = y[1];
    double x_sat = y[2], y_sat = y[3];
    double d_sat_earth = calcDist(xt);
    double d_sat_lune = calcDist(xl);
    double Ep_grav = -G_grav * ms * mt / d_sat_earth - G_grav * ms * ml / d_sat_lune;

    double Ep_centrifuge = -ms * Om * Om * (sq(x_sat) + sq(y_sat)) / 2.0;

    double E_cin = 0.5 * ms * (sq(vx_sat) + sq(vy_sat));
    return Ep_grav + Ep_centrifuge + E_cin;
  }

  void printOut(bool write)
  {
  // TODO calculer l'energie mecanique
    double Energy =  calculateEnergy();

    // Ecriture tous les [sampling] pas de temps, sauf si write est vrai
    if((!write && last>=sampling) || (write && last!=1))
    {
      *outputFile << t << " " << y[0] << " " << y[1] << " " \
      << y[2] << " " << y[3] << " " << Energy << " "<< endl; // write output on file
      last = 1;
    }
    else
    {
      last++;
    }
  }

    void compute_f(valarray<double>& f, const valarray<double>& y_target) //  TODO: Calcule le tableau de fonctions f(y)
  {
      double vx_sat = y_target[0], vy_sat = y_target[1];
      double x_sat = y_target[2], y_sat = y_target[3];
 
      double dist_st = sqrt(sq(x_sat - xt) + sq(y_sat));
      double dist_sl = sqrt(sq(x_sat - xl) + sq(y_sat));
      double dist_st_cube = pow(dist_st, 3.0);
      double dist_sl_cube = pow(dist_sl, 3.0);
      f[0]      = -G_grav * mt * (x_sat - xt) / dist_st_cube - G_grav * ml * (x_sat - xl) / dist_sl_cube + 2 * Om * vy_sat + Om * Om * x_sat;
      f[1]      = -G_grav * mt * y_sat / dist_st_cube - G_grav * ml * y_sat / dist_sl_cube - 2 * Om * vx_sat + Om * Om * y_sat;
      f[2]      = vx_sat; 
      f[3]      = vy_sat; 
    }

    // New step method from EngineEuler
    void step()
    {
      unsigned int iteration=0;
      double error=999e0;
      valarray<double> f_impl =valarray<double>(0.e0,4); 
      valarray<double> f_expl =valarray<double>(0.e0,4); 
      valarray<double> yold=valarray<double>(y);  // y_n
      valarray<double> y_control=valarray<double>(y);  // y^(k+1)
      valarray<double> delta_y_EE=valarray<double>(y);


      //TODO : écrire un algorithme valide pour chaque alpha dans [0,1]
      // tel que alpha=1 correspond à Euler explicite et alpha=0 à Euler implicite 
      // et alpha=0.5 à Euler semi-implicite
      if(alpha >= 0. && alpha <= 1.0){
        t += dt;                 //mise à jour du temps 
        while(error>tol && iteration<=maxit){
          compute_f(f_expl, yold);
          valarray<double> dy_expl = alpha * f_expl;   // Partie explicite
          compute_f(f_impl, y_control);
          valarray<double> dy_impl = (1 - alpha) * f_impl;  // Partie implicite
          valarray<double> y_control_old = valarray<double>(y_control);  // y^(k)
          y_control = yold + dt * (dy_impl + dy_expl);

          delta_y_EE = y_control - y_control_old;
          error = calc_norm(delta_y_EE);
          iteration += 1;
        }
        if(iteration>=maxit){
          cout << "WARNING: maximum number of iterations reached, error: " << error << endl;
        }
      }
      else
      {
        cerr << "alpha not valid" << endl;
      }
      // cout << iteration << endl;
      y = y_control;
    }

public:
    // Modified constructor
    Engine(ConfigFile configFile)
    {
      // Stockage des parametres de simulation dans les attributs de la classe
      tfin     = configFile.get<double>("tfin",tfin);	        // lire le temps final de simulation
      nsteps   = configFile.get<unsigned int>("nsteps",nsteps); // lire le nombre de pas de temps
      y0[0]    = configFile.get<double>("vx0",y0[0]);  // vitesse initiale selon x	    
      y0[1]    = configFile.get<double>("vy0",y0[1]);  // vitesse initiale selon y       
      y0[2]    = configFile.get<double>("x0",y0[2]);   // position initiale selon x       
      y0[3]    = configFile.get<double>("y0",y0[3]);   // position initiale selon y	    
      G_grav   = configFile.get<double>("G_grav",G_grav);           
      ml       = configFile.get<double>("ml",ml);            
      mt       = configFile.get<double>("mt",mt);        
      ms       = configFile.get<double>("ms",ms);
      dist     = configFile.get<double>("dist",dist);        
      sampling = configFile.get<unsigned int>("sampling",sampling);
      tol      = configFile.get<double>("tol", tol);
      maxit    = configFile.get<unsigned int>("maxit", maxit);
      alpha    = configFile.get<double>("alpha", alpha);
      // TODO: calculer le time step
      dt       = tfin / nsteps; 

      
      // Ouverture du fichier de sortie
      outputFile = new ofstream(configFile.get<string>("output","output.out").c_str()); 
      outputFile->precision(15); // Les nombres seront ecrits avec 15 decimales
    };


    // Destructeur virtuel
    virtual ~Engine()
    {
      outputFile->close();
      delete outputFile;
    };
      // Simulation complete
    void run()
    {
      double mass_total = mt + ml;
      // TODO : initialiser la position de la Terre et de la Lune, ainsi que la position de X' du satellite et Omega
      Om = sqrt(G_grav * mass_total / pow(dist, 3));
      //Om = 0;
      xt = -dist * ml / mass_total;
      xl = dist * mt / mass_total;
      //cout << "X lune: " << xl << endl;
      y0[2] = dist * (mt - sqrt(ml * mt)) / (mt - ml) + xt;
      t = 0.e0; // initialiser le temps
      y = y0;   // initialiser le position 
      last = 0; // initialise le parametre d'ecriture

      printOut(true); // ecrire la condition initiale
#ifdef SDL_VISUALIZE
      bool running = true;
      running = initializeSDL(1200, 500);
      while(running){
        drawObjects(xt, xl, y[2], y[3]);
        running = processEventsAndQuit();
         
#else
      for(unsigned int i(0); i<nsteps; ++i) // boucle sur les pas de temps
      {
        printOut(false); // ecrire le pas de temps actuel
#endif
        step();  // faire un pas de temps
      }
      printOut(true); // ecrire le dernier pas de temps

    };
   
};

// programme
int main(int argc, char* argv[])
{
  string inputPath("configuration.in.example"); // Fichier d'input par defaut
  if(argc>1) // Fichier d'input specifie par l'utilisateur ("./Exercice2 config_perso.in")
      inputPath = argv[1];

  ConfigFile configFile(inputPath); // Les parametres sont lus et stockes dans une "map" de strings.

  for(int i(2); i<argc; ++i) // Input complementaires ("./Exercice2 config_perso.in input_scan=[valeur]")
      configFile.process(argv[i]);

  Engine* engine;

  // Create an instance of Engine instead of EngineEuler
  engine = new Engine(configFile);

  engine->run(); // executer la simulation

  delete engine; // effacer la class simulation 
  cout << "Fin de la simulation." << endl;
  return 0;
}


