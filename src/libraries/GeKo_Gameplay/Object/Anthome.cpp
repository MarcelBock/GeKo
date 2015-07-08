#include "GeKo_Gameplay/Object/AntHome.h"

AntHome::AntHome(){

}

AntHome::AntHome(glm::vec3 position, SoundFileHandler *sfh, Geometry antMesh, SoundObserver *soundObserver, ObjectObserver *objectObserver, Node *rootNode, std::vector<std::vector<glm::vec3>> posTreeFoods, Terrain* terrain){
	// ~~~~~~~ Home ~~~~~~~
	m_myNodeName = "Anthome";
	m_position = position;
	m_type = ObjectType::HOUSE;

	Item cookie(1);
	cookie.setName("Cookie");
	cookie.setTypeId(ItemType::COOKIE);
	m_inventory->addItem(&cookie, 100);

	addObserver(objectObserver);
	m_gravity = new Gravity();
	m_sfh = sfh;

	// ~~~~~~~ Ants ~~~~~~~
	m_antMesh = antMesh;

	m_objectObserver = objectObserver;
	m_soundObserver = soundObserver;

	m_workerTexture = new Texture((char*)RESOURCES_PATH "/Texture/ant.jpg");
	m_guardTexture = new Texture((char*)RESOURCES_PATH "/Texture/ant2.jpg");
	m_queenTexture = new Texture((char*)RESOURCES_PATH "/Texture/ant3.jpg");

	m_guardsDistanceToHome = 6;
	setGraphGuards();
	setGraphWorker(posTreeFoods);
	setGrapHighOnTerrain(terrain);

	DecisionTree *aggressivedecisionTree = new DecisionTree();
	aggressivedecisionTree->setAntTreeAggressiv();
	m_guardDecisionTree = aggressivedecisionTree;
	DecisionTree *afraidDecisionTree = new DecisionTree();
	afraidDecisionTree->setAntTreeAfraid();
	m_workerDecisionTree = afraidDecisionTree;

	// ~~~~~~~ Anything else ~~~~~~~
	m_rootNode = rootNode;

	m_numberOfAnts = 0;
	m_numberOfGuards = 0;
	m_numerOfDeadGuards = m_numberOfGuards;
	m_numberOfWorkers = 0;
	m_numberOfDeadWorkers = m_numberOfWorkers;
	m_queenIsAlive = false;
	m_antScale = 0.5;

	// ~~~~~~~ Generate Ants ~~~~~~~
	generateWorkers(1);
	generateGuards(1);
}

AntHome::~AntHome(){

}

void AntHome::generateGuards(int i){
	if (i < 0)
		return;
	glm::vec4 position;
	time_t t;
	time(&t);
	srand((unsigned int)t);
	std::stringstream name;
	while (i > 0){
		name << "Guard" << m_numberOfGuards++ + 1;
		Node *aiGuardNode = new Node(name.str());
		//Node (scenegraph)
		aiGuardNode->addGeometry(&m_antMesh);
		aiGuardNode->addScale(m_antScale, m_antScale, m_antScale);
		aiGuardNode->addTexture(m_guardTexture);
		aiGuardNode->addGravity(m_gravity);
		Ant_Guardian *antAI = new Ant_Guardian();
		antAI->setAntAggressiv(m_numberOfAnts, name.str(), m_guardDecisionTree, m_guardGraph);
		aiGuardNode->setObject(antAI);
		antAI->addObserver(m_objectObserver);
		antAI->setSoundHandler(m_sfh);
		antAI->addObserver(m_soundObserver);
		generateSound(antAI);
		name.str("");
		m_rootNode->addChildrenNode(aiGuardNode);
		m_guards.push_back(aiGuardNode);
		m_numerOfDeadGuards = m_numberOfGuards;
		i--;
		//printPosGuards();
	}
}

void AntHome::generateWorkers(int i){
	if (i < 0)
		return;
	glm::vec4 position;
	time_t t;
	time(&t);

	srand((unsigned int)t);
	std::stringstream name;
	while (i > 0){
		name << "Worker" << m_numberOfWorkers++ + 1;
		Node *aiWorkerNode = new Node(name.str());
		aiWorkerNode->addGeometry(&m_antMesh);
		aiWorkerNode->addScale(m_antScale*0.8, m_antScale*0.8, m_antScale*0.8);
		aiWorkerNode->addTexture(m_workerTexture);
		aiWorkerNode->addGravity(m_gravity);

		Ant_Worker *antAI = new Ant_Worker(glm::vec4(m_position.x, m_position.y + 10, m_position.z, 1.0), (rand()* 5.0f / 32767.0f));
		antAI->setAntAfraid(m_numberOfAnts, name.str(), m_workerDecisionTree, m_afraidGraph);
		aiWorkerNode->setObject(antAI);
		antAI->addObserver(m_objectObserver);
		antAI->setSoundHandler(m_sfh);
		antAI->addObserver(m_soundObserver);
		generateSound(antAI);
		name.str("");
		m_rootNode->addChildrenNode(aiWorkerNode);
		m_workers.push_back(aiWorkerNode);
		m_numberOfDeadWorkers = m_numberOfWorkers;
		i--;
		//printPosWorkers();
	}
}

void AntHome::generateQueen(){
	glm::vec4 position;
	time_t t;
	time(&t);
	srand((unsigned int)t);
	
	//Node (scenegraph)
	m_queen = new Node("Queen");
	m_queen->addGeometry(&m_antMesh);
	m_queen->addScale(m_antScale*2.0, m_antScale*2.0, m_antScale*2.0);
	m_queen->addTexture(m_queenTexture);
	m_queen->addGravity(m_gravity);
	m_queen->addTranslation(glm::vec3(0.0, 2.0, 0.0));
	m_queen->getBoundingSphere()->center.y -= 2.0;

	Ant_Queen *antAI = new Ant_Queen();
	antAI->setAntQueen(m_numberOfAnts, "Queen", m_guardDecisionTree, m_guardGraph);
	m_queen->setObject(antAI);

	antAI->addObserver(m_objectObserver);
	antAI->setSoundHandler(m_sfh);
	antAI->addObserver(m_soundObserver);
	generateSound(antAI);
	m_rootNode->addChildrenNode(m_queen);
	m_queenIsAlive = true;

	notify(*m_queen, Object_Event::OBJECT_ADDED_TO_SCENE);
}

void AntHome::generateSound(AI *ai){
	std::stringstream name;
	name << "AIFootsteps" << m_numberOfAnts + 1;
	std::string namestring;
	namestring = name.str();
	ai->setSourceName(MOVESOUND_AI, namestring, RESOURCES_PATH "/Sound/Footsteps.wav");
	m_sfh->disableLooping(namestring);
	name.str("");
	name << "AIDeath" << m_numberOfAnts + 1;
	namestring = name.str();
	ai->setSourceName(DEATHSOUND_AI, namestring, RESOURCES_PATH "/Sound/death.wav");
	m_sfh->disableLooping(namestring);
	name.str("");
	name << "AIEssen" << m_numberOfAnts + 1;
	namestring = name.str();
	ai->setSourceName(EATSOUND_AI, namestring, RESOURCES_PATH "/Sound/Munching.wav");
	m_sfh->disableLooping(namestring);
	name.str("");
	name << "Flies" << m_numberOfAnts++ + 1;
	namestring = name.str();
	ai->setSourceName(DEATHSOUND_FLIES_AI, namestring, RESOURCES_PATH "/Sound/Fliege kurz.wav");
	//m_sfh->disableLooping(namestring);
	m_sfh->setGain(namestring, 7.0);
	name.str("");
}

void AntHome::updateAnts(){
	for (int i = 0; i < m_guards.size(); i++){
		m_guards.at(i)->getAI()->update();
	}
	for (int i = 0; i < m_workers.size(); i++){
		m_workers.at(i)->getAI()->update();
	}
	if (m_queenIsAlive)
		m_queen->getAI()->update();
	//std::cout << m_queen->getBoundingSphere()->center.x << m_queen->getBoundingSphere()->center.y << m_queen->getBoundingSphere()->center.z << std::endl;
}

void AntHome::printPosGuards(){
	for (int i = 0; i < m_guards.size(); i++){
		std::cout << "Guard " << i << " Pos : x :" << m_guards[i]->getAI()->getPosition().x << " ; z: " << m_guards[i]->getAI()->getPosition().z << std::endl;
	}
}

void AntHome::printPosWorkers(){
	for (int i = 0; i < m_workers.size(); i++){
		std::cout << "Worker " << i << " Pos : x :" << m_workers[i]->getAI()->getPosition().x << " ; z: " << m_workers[i]->getAI()->getPosition().z << std::endl;
	}
}

void AntHome::setAntScale(float scale){
	m_antScale = scale;
}

float AntHome::getAntScale(){
	return m_antScale;
}

void AntHome::setGraphGuards(){
	//TODO: Anpassen der H�henwerte (m_GuardGraph.y) an die H�henwerte vom Terrain anpassen! Sonst erkennt die Ant nicht, dass sie am Ziel angekommen ist
	Graph<AStarNode, AStarAlgorithm>* antAggressiveGraph = new Graph<AStarNode, AStarAlgorithm>();
	antAggressiveGraph->setExampleAntAggressiv(m_position, m_guardsDistanceToHome);
	m_guardGraph = antAggressiveGraph;
}

void AntHome::setGraphWorker(std::vector<std::vector<glm::vec3>> posTreeFoods){
	//TODO: Anpassen der H�henwerte (m_WorkerGraph.y) an die H�henwerte vom Terrain anpassen! Sonst erkennt die Ant nicht, dass sie am Ziel angekommen ist
	Graph<AStarNode, AStarAlgorithm>* antAfraidGraph = new Graph<AStarNode, AStarAlgorithm>();
	antAfraidGraph->setExampleAntAfraid2(m_position, posTreeFoods);
	m_afraidGraph = antAfraidGraph;
}

void AntHome::setGrapHighOnTerrain(Terrain* t){
	for (int i = 0; i < m_guardGraph->getGraph()->size(); i++){
		AStarNode* node = m_guardGraph->getGraph()->at(i);
		glm::vec3 pos = node->getPosition();
		node->setPosition(glm::vec3(pos.x, t->getHeight(glm::vec2(pos.x, pos.z))+1, pos.z));
	}
}

void AntHome::resetDeadGuard(int i){
	//Ant_Guardian guard = m_guards.at(i)->getAI();

	//TODO: Can�t walk after dead, (First fix, that can walk after meet player)
	if (m_guards.size() > 0 && i < m_guards.size()){
		m_guards.at(i)->getAI()->setPosition(glm::vec3(m_position.x - 5, m_position.y, m_position.z + 5));

		m_guards.at(i)->getAI()->setHealth(m_guards.at(i)->getAI()->getHealthMax());
		m_guards.at(i)->getAI()->setHunger(m_guards.at(i)->getAI()->getHungerMax());
		m_numerOfDeadGuards++;
		std::stringstream name;
		name << "Guard" << m_numerOfDeadGuards;
		m_guards.at(i)->getAI()->setName(name.str());
		m_guards.at(i)->getAI()->getInventory()->clearInventory();

		m_guards.at(i)->getAI()->setStates(States::HEALTH, true);
		m_guards.at(i)->getAI()->setHasDied(false);
	}
	std::cout << "m_numerOfDeadGuards : " << m_numerOfDeadGuards << " ; m_numberOfGuards : " << m_numberOfGuards << std::endl;
	if (m_numerOfDeadGuards == m_numberOfGuards+5){
		generateQueen();
	}
}

void AntHome::resetDeadWorker(int i){
	if (m_workers.size() > 0 && i < m_workers.size()){
		m_workers.at(i)->getAI()->setPosition(m_position);
		m_workers.at(i)->getAI()->setHealth(m_workers.at(i)->getAI()->getHealthMax());
		m_workers.at(i)->getAI()->setHunger(m_workers.at(i)->getAI()->getHungerMax());
		m_numberOfDeadWorkers += 1;
		std::stringstream name;
		name << "Worker" << m_numberOfDeadWorkers;
		m_workers.at(i)->getAI()->setName(name.str());
		m_workers.at(i)->getAI()->getInventory()->clearInventory();

		m_workers.at(i)->getAI()->setStates(States::HEALTH, true);
		m_workers.at(i)->getAI()->setHasDied(false);
	}
}

void AntHome::resetDeadAnt(AntType type){
	//TODO: Bisher wird nur die erste Ant wieder respawnt, weil 0 �bergeben wird!
	if (type == AntType::GUARD){
		resetDeadGuard(0);
	}
	if (type == AntType::WORKER){
		resetDeadWorker(0);
	}
}