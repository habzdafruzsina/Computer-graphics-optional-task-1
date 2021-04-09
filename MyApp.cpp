#include "MyApp.h"
#include "GLUtils.hpp"

#include <math.h>

CMyApp::CMyApp(void)
{
	m_vaoID = 0;
	m_vboID = 0;
	m_programID = 0;
}


CMyApp::~CMyApp(void)
{
}

bool CMyApp::Init()
{
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//glCullFace(GL_BACK);

	glPolygonMode(GL_BACK, GL_LINE);

	
	/** /
	Vertex vert[] =
	{ 
		// 1. háromszög
		//          x,  y, z             R, G, B
		{glm::vec3(-1, -1, 0), glm::vec3(1, 0, 0)},
		{glm::vec3( 1, -1, 0), glm::vec3(0, 1, 0)},
		{glm::vec3(-1,  1, 0), glm::vec3(0, 0, 1)},

		// 2. háromszög
		{glm::vec3(-1,  1, 0), glm::vec3(0, 0, 1)},
		{glm::vec3( 1, -1, 0), glm::vec3(0, 1, 0)},
		{glm::vec3( 1,  1, 0), glm::vec3(1, 1, 1)},
	};
	/**/

	////////
	Vertex A = { glm::vec3(-1,0, 1), glm::vec3(1,0,0) };
	Vertex B = { glm::vec3( 1,0, 1), glm::vec3(0,1,0) };
	Vertex C = { glm::vec3(-1,0,-1), glm::vec3(0,0,1) };
	Vertex D = { glm::vec3( 1,0,-1), glm::vec3(1,1,1) };
	Vertex E = { glm::vec3( 0, sqrt(2.0f), 0), glm::vec3(1,0,1) };

	Vertex vert[] =
	{
		A, B, E,
		B, D, E,
		D, C, E,
		C, A, E,
		B, A, C,
		B, C, D
	};


	glGenVertexArrays(1, &m_vaoID);
	glBindVertexArray(m_vaoID);
	

	glGenBuffers(1, &m_vboID); 
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID); 
	///////////
	glBufferData( GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

	///////////
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0); 
	///////////
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)) );

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	GLuint vs_ID = loadShader(GL_VERTEX_SHADER,		"myVert.vert");
	GLuint fs_ID = loadShader(GL_FRAGMENT_SHADER,	"myFrag.frag");

	m_programID = glCreateProgram();

	glAttachShader(m_programID, vs_ID);
	glAttachShader(m_programID, fs_ID);

	glBindAttribLocation( m_programID, 0, "vs_in_pos");
	glBindAttribLocation( m_programID, 1, "vs_in_col");

	glLinkProgram(m_programID);

	GLint infoLogLength = 0, result = 0;

	glGetProgramiv(m_programID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( GL_FALSE == result )
	{
		std::vector<char> ProgramErrorMessage( infoLogLength );
		glGetProgramInfoLog(m_programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
		
		char* aSzoveg = new char[ProgramErrorMessage.size()];
		memcpy( aSzoveg, &ProgramErrorMessage[0], ProgramErrorMessage.size());

		std::cout << "[app.Init()] Sáder Huba panasza: " << aSzoveg << std::endl;

		delete aSzoveg;
	}

	glDeleteShader( vs_ID );
	glDeleteShader( fs_ID );


	m_matProj = glm::perspective( 45.0f, 640/480.0f, 1.0f, 1000.0f );

	m_loc_world = glGetUniformLocation( m_programID, "world");
	m_loc_view  = glGetUniformLocation( m_programID, "view" );
	m_loc_proj  = glGetUniformLocation( m_programID, "proj" );

	return true;
}

void CMyApp::Clean()
{
	glDeleteBuffers(1, &m_vboID);
	glDeleteVertexArrays(1, &m_vaoID);

	glDeleteProgram( m_programID );
}

void CMyApp::Update()
{
	m_matView = glm::lookAt(glm::vec3( 0,  2,  5),
							glm::vec3( 0,  0,  0),
							glm::vec3( 0,  1,  0));
	/** /
	float time = SDL_GetTicks() / 1000.0f;
	float r = 5;
	// 3 mp alatt teszünk meg egy kört:
	float alpha = time * 2 * M_PI / 3.f;
	m_matView = glm::lookAt(glm::vec3( r*cos(alpha), -2.f,  r*sin(alpha)),		// honnan nézzük a színteret
							glm::vec3( 0,  0,  0),		// a színtér melyik pontját nézzük
							glm::vec3( 0,  1,  0));		// felfelé mutató irány a világban
	/**/
}


void CMyApp::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram( m_programID );

	glBindVertexArray(m_vaoID);
	glUniformMatrix4fv(m_loc_view, 1, GL_FALSE, &(m_matView[0][0]));
	glUniformMatrix4fv(m_loc_proj, 1, GL_FALSE, &(m_matProj[0][0]));


	/*
	GLM transzformációs mátrixokra példák:
		glm::rotate<float>( szög, glm::vec3(tengely_x, tengely_y, tengely_z) ) <- tengely_{xyz} körüli elforgatás
		glm::translate<float>( glm::vec3(eltol_x, eltol_y, eltol_z) ) <- eltolás
		glm::scale<float>( glm::vec3(s_x, s_y, s_z) ) <- léptékezés

	*/

	float time = SDL_GetTicks() / 1000.0f;
	float alpha = time * 2 * M_PI / 3.f;

	m_matWorld = 
		glm::rotate<float>(alpha, glm::vec3(0, 1, 0));

	glUniformMatrix4fv( m_loc_world, 1, GL_FALSE, &(m_matWorld[0][0]) );

	glDrawArrays( GL_TRIANGLES, 0, 6*3);


	int n = 5;
	for (int i = 0; i < n; i++) {
		m_matWorld =

			glm::rotate<float>(2 * M_PI / n * i, glm::vec3(0, 1, 0)) *  //egyenlõ távolsára legyenek az objektumok
			glm::rotate<float>(alpha, glm::vec3(0, 1, 0)) *		// a középpont körül is forogjon
			glm::translate<float>(glm::vec3(3, 0, 0)) *			// távolabb helyezzük a középponttól
			glm::rotate<float>(-alpha*3, glm::vec3(0, 1, 0)) *	// tengelye körül forogjon visszafelé
			glm::scale<float>(glm::vec3(0.5f, 0.5f, 0.5f))		// átméretezés
			;

		glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(m_matWorld[0][0]));

		glDrawArrays(GL_TRIANGLES, 0, 6 * 3);
	}


	//m_matWorld = glm::translate<float>(glm::vec3(-1, 0, 0))
	//	* glm::rotate<float>(alpha, glm::vec3(0, 1, 0));
	//glUniformMatrix4fv(m_loc_world, 1, GL_FALSE, &(m_matWorld[0][0]));
	//glDrawArrays(GL_TRIANGLES, 0, 6 * 3);


	glBindVertexArray(0);

	glUseProgram( 0 );
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{

}

void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseWheel(SDL_MouseWheelEvent& wheel)
{
}


void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_matProj = glm::perspective( 45.0f, _w/(float)_h, 0.01f, 100.0f);
}