//Titan Engine, by Atlas X Games 
// Sound.h - header for the classes that allow for audio in applications
#pragma once

// Include Fmod
#include "fmod_studio.hpp"
#include "fmod_errors.h"

//include titan's precompiled header
#include "Titan/ttn_pch.h"

//Class for an audio object, the other classes inherit from this
class TTN_AudioObject // Has helpers for audio classes
{

public:
	virtual ~TTN_AudioObject() {}

protected:

	int ErrorCheck(FMOD_RESULT result);

	float dbToVolume(float db);
	float VolumeTodb(float volume);
	FMOD_VECTOR VectorToFmod(const glm::vec3& vec);
	glm::vec3 FmodToVector(const FMOD_VECTOR& vec);

};

//Class for Audio buses
class TTN_AudioBus : public TTN_AudioObject
{
	friend class TTN_AudioEngine;
public:
	// Get and Set Paused
	bool CheckPaused();
	void SetPaused(const bool& pause);

	// Get and Set Volume
	float GetVolume();
	void SetVolume(const float& newVolume);

	// Get and Set Mute
	bool CheckMuted();
	void SetMute(const bool& mute);

	// Stop all events
	void StopAllEvent(const bool& fade = false);

private:
	// AudioEngine class uses this to create bus objects
	TTN_AudioBus(FMOD::Studio::Bus* bus);

	// Don't want copies, should only grab refs from audio engine
	TTN_AudioBus(TTN_AudioBus const&) = delete;
	void operator=(TTN_AudioBus const&) = delete;

private:
	FMOD::Studio::Bus* m_Bus;
};


//Class for audio listener, an object that can hear sounds in the world
class TTN_AudioListener : public TTN_AudioObject
{
	friend class TTN_AudioEngine;
public:
	void SetPosition(const glm::vec3& pos);
	glm::vec3 GetPosition();

	// Vel not working
	void SetVelocity(const glm::vec3& vel);
	glm::vec3 GetVelocity();

	// TODO: Test these
	void SetForward(const glm::vec3& forward);
	glm::vec3 GetForward();
	void SetUp(const glm::vec3& up);
	glm::vec3 GetUp();

private:
	// Only AudioEngine can create a listener
	// Get a ref from AudioEngine::GetListener()
	TTN_AudioListener() {}
	TTN_AudioListener(TTN_AudioListener const&) = delete;
	void operator=(TTN_AudioListener const&) = delete;

private:
	// Ref to the FMOD System
	FMOD::Studio::System* m_StudioSystem;

	// Save the most recent changes
	FMOD_3D_ATTRIBUTES m_Attributes;
	FMOD_VECTOR m_AttenuationPosition;

	// Basic ID, first listener is 0
	int m_ID;
	void SetID(const int& id);
};

//Class for an audio event or source of sound in the world
class TTN_AudioEvent : public TTN_AudioObject
{
	friend class TTN_AudioEngine;
public:

	~TTN_AudioEvent();

	// Will only play if event is not currently playing
	void Play();

	// Restarts the event
	void Restart();

	// Allows AHDSR modulators to complete their release, and DSP effect tails to play out.
	void Stop();

	// Stops the event instance immediately.
	void StopImmediately();

	// Checks if event is playing
	bool isPlaying();

	// Parameters
	void SetParameter(const char* name, const float& value, const bool& ignoreSeekSpeed = false);
	float GetParameterValue(const char* name);

	void SetPosition(const glm::vec3& pos);
	glm::vec3 GetPosition();

private:
	// AudioEngine class uses this to create Event objects
	TTN_AudioEvent(FMOD::Studio::EventInstance* eventInstance);

	// Don't want copies, should only grab refs from audio engine
	TTN_AudioEvent(TTN_AudioEvent const&) = delete;
	void operator=(TTN_AudioEvent const&) = delete;

private:
	FMOD_3D_ATTRIBUTES m_Attributes;
	FMOD::Studio::EventInstance* m_EventInstance;
};

//Class for the audio engine singleton 
class TTN_AudioEngine : public TTN_AudioObject
{
	friend class TTN_AudioEngine;
public:
	//// Singleton ///////////////////
	static TTN_AudioEngine& Instance();

	TTN_AudioEngine(TTN_AudioEngine const&) = delete;
	void operator=(TTN_AudioEngine const&) = delete;
	//////////////////////////////////

	void Init();
	void Update();
	void Shutdown();

	//// Banks ////
	void LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags = FMOD_STUDIO_LOAD_BANK_NORMAL);

	//// Listener ////
	TTN_AudioListener& GetListener();

	//// Events ////
	TTN_AudioEvent& MakeEvent(const std::string& strEventName, const std::string& GUID);
	TTN_AudioEvent& GetEvent(const std::string& strEventName);

	//// Global Parameters ////
	void SetGlobalParameter(const char* name, const float& value, const bool& ignoreSeekSpeed = false);
	float GetGlobalParameterValue(const char* name);

	//// Bus ////
	void LoadBus(const std::string& strBusName, const std::string& GUID);
	TTN_AudioBus& GetBus(const std::string& strBusName);


private:
	TTN_AudioEngine() {}

private:
	// FMOD Systems
	FMOD::Studio::System* m_StudioSystem;
	FMOD::System* m_System;

	// Listener
	TTN_AudioListener m_Listener;

	// Banks
	std::unordered_map<std::string, FMOD::Studio::Bank*> m_BankMap;

	// Events
	std::unordered_map<std::string, TTN_AudioEvent*> m_EventMap;

	// Bus
	std::unordered_map<std::string, TTN_AudioBus*> m_BusMap;
};

class TTN_AudioEventHolder {
public:
	//define a special name for shared (smart) pointers to this class
	typedef std::shared_ptr<TTN_AudioEventHolder> saehptr;

	//create a pointer to a new object of this class
	static saehptr Create() {
		return std::make_shared<TTN_AudioEventHolder>();
	}

	//create a pointer to a new object of this class
	static saehptr Create(const std::string& EventName, const std::string& GUID, unsigned num) {
		return std::make_shared<TTN_AudioEventHolder>(EventName, GUID, num);
	}

public:
	//default constructor
	TTN_AudioEventHolder();
	//constructor that takes data
	TTN_AudioEventHolder(const std::string& EventName, const std::string& GUID, unsigned num);

	//Sets the next position
	void SetNextPostion(glm::vec3 pos) { m_nextPosition = pos; }
	//Gets the next positions
	glm::vec3 GetNextPosition() { return m_nextPosition; }

	//Plays the next effect from the queue
	void PlayFromQueue();

	//Gets a reference to the next event in the qeuue
	std::string GetNextEvent() { return m_events.front(); }

private:
	//queue of all the copies of references to the effect
	std::queue<std::string> m_events;
	//position that the next event should be played at
	glm::vec3 m_nextPosition;

	//engine reference
	TTN_AudioEngine& engine = TTN_AudioEngine::Instance();
};