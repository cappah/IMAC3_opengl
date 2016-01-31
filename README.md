# IMAC3_opengl

Voici le repo sur lequel je suis les cours d'Adrien Herubel, à l'IMAC (adrien.io). 

J'ai décidé de partir de ces cours pour me faire un véritable éditeurde scènes.


Commandes de la camera : 
En mode editeur : 
pan : maintenir alt + shift + clic gauche.
turn : maintenir alt + clic gauche.
zoom : maintenir alt + clic droit.

En mode FPS : 
translation : les touches z,q,s,d.
rotation : maintenir le clic droit.

Pour passer d'un mode à l'autre il faut aller dans l'onglet camera mode de la barre du menu, et choisir la camera que l'on souhaite.

Quelques captures d'écran : 
![alt tag](/../td2-assimp/example_01.jpg?raw=true "example 01")
![alt tag](/../td2-assimp/example_02.jpg?raw=true "example 02")

DONE : 

- Architecture Entité / composants

- Forward et deffered shading (avec cohabitation)

- Spot lights, directional lights, spot lights

- Edition de terrain avec génération à partir d'un bruit de perlin (reprit sur ce tutoriel : https://openclassrooms.com/courses/bruits-et-nombres-aleatoires-coherents)

- Implémentation de normal mapping

- Implémentation de skybox avec des cube maps

- Edition des éléments de la scène (déplacement, modification des paramètres) grace à une interface graphique (Dear ImGui)

- Chargement de models 3D / textures / skybox grace à un système de factories. 


TODO : 

- Faire en sorte que les nouveaux objets créé dans la scène soient créés face à la caméra et non en 0,0,0. 

- Pouvoir rapidement centrer la vue sur un objet.

- Redimensionner les colliders lors du chargement d'un objet 3D avec assimp, pour que le collider englobe l'objet.

- Implementer les ombres (cf : tp03).

- Ajouter un système de sérialisation pour sauvegarder et charger une scène. 

- Ajouter de la physique (particule et drapeau, cf : cours d'animation et physique).

- Meilleurs gestion des entitées / composants (pouvoir ajouter/enlever des composants sur les entités dans l'éditeur, pouvoir faire des prefabs,... )

- Pouvoir peindre des textures sur le terrain.

- Effets opengl avancés (cf : tp04)
