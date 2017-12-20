#ifndef PHYSICS2_H_INCLUDED
#define PHYSICS2_H_INCLUDED

#include "Vector2D.h"

class Mass
{
public:
	float m;
	Vector2D pos;
	Vector2D vel;
	Vector2D force;

	Mass( float m ) {
		this->m = m;
	}

	void applyForce( Vector2D force ) {
		this->force += force;
	}

	void init() {
		force.x = 0;
		force.y = 0;
	}

	void simulate( float dt ){
		vel += (force / m) * dt;
		pos += vel * dt;
	}
};

class Simulation
{
public:
	int numOfMasses;
	Mass** masses;

	Simulation( int numOfMasses, float m ) {
		this->numOfMasses = numOfMasses;
		masses = new Mass*[numOfMasses];
		for ( int a = 0; a < numOfMasses; ++a )
			masses[a] = new Mass(m);
	}

	virtual void release() {
		for ( int a = 0; a < numOfMasses; ++a ) {
			delete(masses[a]);
			masses[a] = NULL;
		}
		delete( masses );
		masses = NULL;
	}

	Mass* getMass( int index ) {
		if ( index < 0 || index >= numOfMasses )
			return NULL;
		return masses[index];
	}

	virtual void init() {
		for ( int a = 0; a < numOfMasses; ++a )
			masses[a]->init();
	}

	virtual void solve(){
	}

	virtual void simulate( float dt ) {
		for ( int a = 0; a < numOfMasses; ++a )
			masses[a]->simulate(dt);
	}

	virtual void operate( float dt ) {
		init();
		solve();
		simulate( dt );
	}
};

class Spring
{
public:
	Mass* mass1;
	Mass* mass2;
	float springConstant;
	float springLength;
	float frictionConstant;

	Spring( Mass* mass1, Mass* mass2, float springConstant, float springLength, float frictionConstant ) {
		this->springConstant = springConstant;
		this->springLength = springLength;
		this->frictionConstant = frictionConstant;
		this->mass1 = mass1;
		this->mass2 = mass2;
	}

	void solve() {
		Vector2D springVector = mass1->pos - mass2->pos;
		float r = springVector.length();
		Vector2D force;
		if ( r != 0 )
			force += ( springVector / r ) * ( r - springLength ) * ( -springConstant );
		force += -( mass1->vel - mass2->vel ) * frictionConstant;
		mass1->applyForce( force );
		mass2->applyForce( -force );
	}
};

class RopeSimulation : public Simulation
{
public:
	Spring** springs;
	Vector2D gravitation;
	Vector2D ropeConnectionPos;
	Vector2D ropeConnectionVel;
	float airFrictionConstant;

	RopeSimulation(
		int numOfMasses,
		float m,
		float springConstant,
		float springLength,
		float springFrictionConstant,
		Vector2D gravitation,
		float airFrictionConstant
		) : Simulation( numOfMasses, m ) {

		this->gravitation = gravitation;
		this->airFrictionConstant = airFrictionConstant;

		for ( int a = 0; a < numOfMasses; ++a ) {
			masses[a]->pos.x = a * springLength;
			masses[a]->pos.y = 0;
		}
		springs = new Spring*[numOfMasses - 1];
		for ( int a = 0; a < numOfMasses - 1; ++a ) {
			springs[a] = new Spring( masses[a], masses[a + 1],
				springConstant, springLength, springFrictionConstant );
		}
	}

	void release() {
		Simulation::release();
		for ( int a = 0; a < numOfMasses - 1; ++a ) {
			delete( springs[a] );
			springs[a] = NULL;
		}
		delete( springs );
		springs = NULL;
	}

	void solve() {
		for ( int a = 0; a < numOfMasses - 1; ++a ) {
			springs[a]->solve();
		}
		for ( int a = 0; a < numOfMasses; ++a ) {
			masses[a]->applyForce( gravitation * masses[a]->m );
			masses[a]->applyForce( -masses[a]->vel * airFrictionConstant );
		}
	}

	void simulate( float dt ) {
		Simulation::simulate( dt );
		ropeConnectionPos += ropeConnectionVel * dt;
		masses[0]->pos = ropeConnectionPos;
		masses[0]->vel = ropeConnectionVel;
	}

	void setRopeConnectionVel( Vector2D ropeConnectionVel ){
		this->ropeConnectionVel = ropeConnectionVel;
	}
};

#endif // PHYSICS2_H_INCLUDED
