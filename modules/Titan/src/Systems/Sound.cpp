//Titan Engine, by Atlas X Games 
// Application.cpp - source file fpr the classes that allow for audio in applications

#include "Titan/ttn_pch.h"
#include "Titan/Systems/Sound.h"

////////////////////////////////////
//			 TTN_AudioObject		  //
////////////////////////////////////

FMOD_VECTOR TTN_AudioObject::VectorToFmod(const glm::vec3& vec)
{
	FMOD_VECTOR fVec;
	fVec.x = vec.x;
	fVec.y = vec.y;
	fVec.z = vec.z;
	return fVec;
}

glm::vec3 TTN_AudioObject::FmodToVector(const FMOD_VECTOR& vec)
{
	glm::vec3 gVec;
	gVec.x = vec.x;
	gVec.y = vec.y;
	gVec.z = vec.z;
	return gVec;
}


float TTN_AudioObject::dbToVolume(float db)
{
	return powf(10.0f, 0.05f * db);
}

float TTN_AudioObject::VolumeTodb(float volume)
{
	return 20.0f * log10f(volume);
}


int TTN_AudioObject::ErrorCheck(FMOD_RESULT result)
{

#ifdef _DEBUG

	// Outputs FMOD error message
	if (result != FMOD_OK)
	{
		std::cout << "FMOD ERROR: " << FMOD_ErrorString(result) << std::endl;

		__debugbreak();

		return 1;
	}

#endif // DEBUG

	// All good
	return 0;
}

////////////////////////////////////
//			 TTN_AudioBus   		  //
////////////////////////////////////

TTN_AudioBus::TTN_AudioBus(FMOD::Studio::Bus* bus)
	: m_Bus(bus)
{
}

bool TTN_AudioBus::CheckPaused()
{
	bool paused;
	ErrorCheck(m_Bus->getPaused(&paused));
	return paused;
}

void TTN_AudioBus::SetPaused(const bool& pause)
{
	ErrorCheck(m_Bus->setPaused(pause));
}

float TTN_AudioBus::GetVolume()
{
	float volume;
	ErrorCheck(m_Bus->getVolume(&volume));
	return volume;
}
void TTN_AudioBus::SetVolume(const float& newVolume)
{
	ErrorCheck(m_Bus->setVolume(newVolume));
}

bool TTN_AudioBus::CheckMuted()
{
	bool muted;
	ErrorCheck(m_Bus->getMute(&muted));
	return muted;
}

void TTN_AudioBus::SetMute(const bool& mute)
{
	ErrorCheck(m_Bus->setMute(mute));
}

void TTN_AudioBus::StopAllEvent(const bool& fade)
{
	if (fade)
	{
		ErrorCheck(m_Bus->stopAllEvents(FMOD_STUDIO_STOP_ALLOWFADEOUT));
	}
	else
	{
		ErrorCheck(m_Bus->stopAllEvents(FMOD_STUDIO_STOP_IMMEDIATE));
	}

}



////////////////////////////////////
//			 TTN_AudioListener		  //
////////////////////////////////////

void TTN_AudioListener::SetPosition(const glm::vec3& pos)
{

	// Convert glm vec to fmod vec
	FMOD_VECTOR newPos = VectorToFmod(pos);

	// Update our member data
	m_AttenuationPosition = newPos;
	m_Attributes.position = newPos;

	// Update in fmod studio
	ErrorCheck(
		m_StudioSystem->setListenerAttributes(
			m_ID,
			&m_Attributes,
			&m_AttenuationPosition));

	// Go nap

}

glm::vec3 TTN_AudioListener::GetPosition()
{
	return FmodToVector(m_Attributes.position);
}

void TTN_AudioListener::SetVelocity(const glm::vec3& vel)
{

	// Convert glm vec to fmod vec
	FMOD_VECTOR newVel = VectorToFmod(vel);

	// Update our member data
	m_Attributes.velocity = newVel;

	// Update in fmod studio
	ErrorCheck(
		m_StudioSystem->setListenerAttributes(
			m_ID,
			&m_Attributes,
			&m_AttenuationPosition));
}

glm::vec3 TTN_AudioListener::GetVelocity()
{
	return FmodToVector(m_Attributes.velocity);
}

void TTN_AudioListener::SetForward(const glm::vec3& forward)
{

	// Convert glm vec to fmod vec
	FMOD_VECTOR newForward = VectorToFmod(forward);

	// Update our member data
	m_Attributes.forward = newForward;

	// Update in fmod studio
	ErrorCheck(
		m_StudioSystem->setListenerAttributes(
			m_ID,
			&m_Attributes,
			&m_AttenuationPosition));
}

glm::vec3 TTN_AudioListener::GetForward()
{
	return FmodToVector(m_Attributes.forward);
}

void TTN_AudioListener::SetUp(const glm::vec3& up)
{

	// Convert glm vec to fmod vec
	FMOD_VECTOR newUp = VectorToFmod(up);

	// Update our member data
	m_Attributes.up = newUp;

	// Update in fmod studio
	ErrorCheck(
		m_StudioSystem->setListenerAttributes(
			m_ID,
			&m_Attributes,
			&m_AttenuationPosition));
}

glm::vec3 TTN_AudioListener::GetUp() // no
{
	return FmodToVector(m_Attributes.up);
}



void TTN_AudioListener::SetID(const int& id)
{
	m_ID = id;
}


////////////////////////////////////
//			 TTN_AudioEvent			  //
////////////////////////////////////

TTN_AudioEvent::TTN_AudioEvent(FMOD::Studio::EventInstance* eventInstance)
	: m_EventInstance(eventInstance)
{
}

TTN_AudioEvent::~TTN_AudioEvent()
{
}

void TTN_AudioEvent::Play()
{

	if (isPlaying())
	{
		return;
	}

	// Start the event
	ErrorCheck(m_EventInstance->start());
}


void TTN_AudioEvent::Restart()
{
	// Start the event
	ErrorCheck(m_EventInstance->start());
}

void TTN_AudioEvent::Stop()
{
	ErrorCheck(m_EventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT));
}

void TTN_AudioEvent::StopImmediately()
{
	ErrorCheck(m_EventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE));
}

bool TTN_AudioEvent::isPlaying()
{
	// Check if already playing
	FMOD_STUDIO_PLAYBACK_STATE state;
	ErrorCheck(m_EventInstance->getPlaybackState(&state));

	if (state == FMOD_STUDIO_PLAYBACK_PLAYING)
	{
		return true;
	}

	return false;
}

void TTN_AudioEvent::SetParameter(const char* name, const float& value, const bool& ignoreSeekSpeed)
{
	ErrorCheck(m_EventInstance->setParameterByName(name, value, ignoreSeekSpeed));
}

float TTN_AudioEvent::GetParameterValue(const char* name)
{
	// Make float for output
	float value;

	// Put value into float
	ErrorCheck(m_EventInstance->getParameterByName(name, &value));

	// Return float
	return value;
}

void TTN_AudioEvent::SetPosition(const glm::vec3& pos)
{
	// Update our attributes
	m_Attributes.position = VectorToFmod(pos);

	// Send update to fmod
	ErrorCheck(m_EventInstance->set3DAttributes(&m_Attributes));
}

glm::vec3 TTN_AudioEvent::GetPosition()
{
	return FmodToVector(m_Attributes.position);
}

////////////////////////////////////
//			 Audio Engine		  //
////////////////////////////////////

TTN_AudioEngine& TTN_AudioEngine::Instance()
{
	static TTN_AudioEngine instance;

	return instance;
}

void TTN_AudioEngine::Init()
{
	// Initilizs the audio systems in FMOD
	ErrorCheck(FMOD::Studio::System::create(&m_StudioSystem));
	ErrorCheck(m_StudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

	// Save ref to core system
	ErrorCheck(m_StudioSystem->getCoreSystem(&m_System));

	// Set one listener, makes its ID 0
	ErrorCheck(m_StudioSystem->setNumListeners(1));
	m_Listener.SetID(0);
	m_Listener.m_StudioSystem = m_StudioSystem;

	// Get the attributes
	ErrorCheck(
		m_StudioSystem->getListenerAttributes(0,
			&m_Listener.m_Attributes,
			&m_Listener.m_AttenuationPosition));

}

void TTN_AudioEngine::Update()
{
	// Update FMOD system
	ErrorCheck(m_StudioSystem->update());

}

void TTN_AudioEngine::Shutdown()
{
	// Delete events
	for (auto event : m_EventMap)
	{
		delete event.second;
	}
	m_EventMap.clear();

	// Delete busses
	for (auto bus : m_BusMap)
	{
		delete bus.second;
	}
	m_BusMap.clear();

	TTN_AudioEngine::ErrorCheck(m_StudioSystem->unloadAll());
	TTN_AudioEngine::ErrorCheck(m_StudioSystem->release());
}

void TTN_AudioEngine::LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{

	// Make sure bank isn't already loaded
	auto bank = m_BankMap.find(bankName);
	if (bank != m_BankMap.end())
		return;

	// Create new bank
	FMOD::Studio::Bank* newBank;

	// Load the bank with FMOD
	ErrorCheck(m_StudioSystem->loadBankFile((bankName + ".bank").c_str(), flags, &newBank));


	// Store the bank in our map so we can use it later
	if (newBank)
	{
		m_BankMap[bankName] = newBank;
	}
}

TTN_AudioListener& TTN_AudioEngine::GetListener()
{
	return m_Listener;
}

TTN_AudioEvent& TTN_AudioEngine::CreateEvent(const std::string& eventName, const std::string& GUID)
{
	// Get find event in file
	FMOD::Studio::EventDescription* eventDescription = NULL;
	ErrorCheck(m_StudioSystem->getEvent(GUID.c_str(), &eventDescription));

	// If the event exists
	if (eventDescription)
	{
		// Use the event description to make and return an instance
		FMOD::Studio::EventInstance* newFMODEvent = nullptr;
		ErrorCheck(eventDescription->createInstance(&newFMODEvent));

		// Create an audio event
		TTN_AudioEvent* newTTN_AudioEvent = new TTN_AudioEvent(newFMODEvent);

		// Make sure multiple events with the same name aren't created
		if (m_EventMap.find(eventName) != m_EventMap.end())
		{
			__debugbreak();
		}

		// Add the audio event to our map
		m_EventMap[eventName] = newTTN_AudioEvent;

		return *newTTN_AudioEvent;
	}

}

TTN_AudioEvent& TTN_AudioEngine::GetEvent(const std::string& strEventName)
{
	// If the event exists
	if (m_EventMap.find(strEventName) != m_EventMap.end())
	{
		// Return it
		return *m_EventMap.at(strEventName);
	}
	else
	{
		// Event not found
		__debugbreak;
	}
}

void TTN_AudioEngine::LoadBus(const std::string& strBusName, const std::string& GUID)
{
	// Find the bus in fmod
	FMOD::Studio::Bus* fmodBus = nullptr;
	ErrorCheck(m_StudioSystem->getBus(GUID.c_str(), &fmodBus));

	// Make sure we're not adding a duplicate name
	if (m_BusMap.find(strBusName) != m_BusMap.end())
	{
		__debugbreak();
	}

	// Create an audio event
	TTN_AudioBus* newBus = new TTN_AudioBus(fmodBus);

	// Add the bus to our map
	m_BusMap[strBusName] = newBus;

	// Close your eyes for a bit, you deserve it
}

TTN_AudioBus& TTN_AudioEngine::GetBus(const std::string& strBusName)
{
	// If the bus exists
	if (m_BusMap.find(strBusName) != m_BusMap.end())
	{
		// Return it
		return *m_BusMap.at(strBusName);
	}
	else
	{
		// Bus not found
		__debugbreak;
	}
}

/////// Global Parameters //////////

void TTN_AudioEngine::SetGlobalParameter(const char* name, const float& value, const bool& ignoreSeekSpeed)
{
	ErrorCheck(m_StudioSystem->setParameterByName(name, value, ignoreSeekSpeed));
}

float TTN_AudioEngine::GetGlobalParameterValue(const char* name)
{
	// Make float for output
	float value;

	// Put value into float
	ErrorCheck(m_StudioSystem->getParameterByName(name, &value));

	// Return float
	return value;
}





