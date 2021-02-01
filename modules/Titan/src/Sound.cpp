#include "Titan/ttn_pch.h"

#include "Titan/Sound.h"

////////////////////////////////////
//			 AudioObject		  //
////////////////////////////////////

FMOD_VECTOR AudioObject::VectorToFmod(const glm::vec3& vec)
{
	FMOD_VECTOR fVec;
	fVec.x = vec.x;
	fVec.y = vec.y;
	fVec.z = vec.z;
	return fVec;
}

glm::vec3 AudioObject::FmodToVector(const FMOD_VECTOR& vec)
{
	glm::vec3 gVec;
	gVec.x = vec.x;
	gVec.y = vec.y;
	gVec.z = vec.z;
	return gVec;
}


float AudioObject::dbToVolume(float db)
{
	return powf(10.0f, 0.05f * db);
}

float AudioObject::VolumeTodb(float volume)
{
	return 20.0f * log10f(volume);
}


int AudioObject::ErrorCheck(FMOD_RESULT result)
{

	// Outputs FMOD error message
	if (result != FMOD_OK)
	{
		std::cout << "FMOD ERROR: " << FMOD_ErrorString(result) << std::endl;

		__debugbreak();

		return 1;
	}

	// All good
	return 0;
}

////////////////////////////////////
//			 AudioBus   		  //
////////////////////////////////////

AudioBus::AudioBus(FMOD::Studio::Bus* bus)
	: m_Bus(bus)
{
}

bool AudioBus::CheckPaused()
{
	bool paused;
	ErrorCheck(m_Bus->getPaused(&paused));
	return paused;
}

void AudioBus::SetPaused(const bool& pause)
{
	ErrorCheck(m_Bus->setPaused(pause));
}

float AudioBus::GetVolume()
{
	float volume;
	ErrorCheck(m_Bus->getVolume(&volume));
	return volume;
}
void AudioBus::SetVolume(const float& newVolume)
{
	ErrorCheck(m_Bus->setVolume(newVolume));
}

bool AudioBus::CheckMuted()
{
	bool muted;
	ErrorCheck(m_Bus->getMute(&muted));
	return muted;
}

void AudioBus::SetMute(const bool& mute)
{
	ErrorCheck(m_Bus->setMute(mute));
}

void AudioBus::StopAllEvent(const bool& fade)
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
//			 AudioListener		  //
////////////////////////////////////

void AudioListener::SetPosition(const glm::vec3& pos)
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

glm::vec3 AudioListener::GetPosition()
{
	return FmodToVector(m_Attributes.position);
}

void AudioListener::SetVelocity(const glm::vec3& vel)
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

glm::vec3 AudioListener::GetVelocity()
{
	return FmodToVector(m_Attributes.velocity);
}

void AudioListener::SetForward(const glm::vec3& forward)
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

glm::vec3 AudioListener::GetForward()
{
	return FmodToVector(m_Attributes.forward);
}

void AudioListener::SetUp(const glm::vec3& up)
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

glm::vec3 AudioListener::GetUp() // no
{
	return FmodToVector(m_Attributes.up);
}



void AudioListener::SetID(const int& id)
{
	m_ID = id;
}


////////////////////////////////////
//			 AudioEvent			  //
////////////////////////////////////

AudioEvent::AudioEvent(FMOD::Studio::EventInstance* eventInstance)
	: m_EventInstance(eventInstance)
{
}

AudioEvent::~AudioEvent()
{
}

void AudioEvent::Play()
{

	// Check if already playing
	FMOD_STUDIO_PLAYBACK_STATE* state = NULL;

	if (m_EventInstance->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING)
	{
		return;
	}

	// Start the event
	m_EventInstance->start();
}


void AudioEvent::Restart()
{
	// Start the event
	m_EventInstance->start();
}

void AudioEvent::Stop()
{
	// Check if already playing
	FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
	if (m_EventInstance->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING)
	{
		// Stop the event
		m_EventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
	}
}

void AudioEvent::StopImmediately()
{
	// Check if already playing
	FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
	if (m_EventInstance->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING)
	{
		// Stop the event
		m_EventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	}
}

bool AudioEvent::isPlaying()
{
	// Check if already playing
	FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
	if (m_EventInstance->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING)
	{
		return true;
	}

	return false;
}

void AudioEvent::SetParameter(const char* name, const float& value, const bool& ignoreSeekSpeed)
{
	ErrorCheck(m_EventInstance->setParameterByName(name, value, ignoreSeekSpeed));
}

float AudioEvent::GetParameterValue(const char* name)
{
	// Make float for output
	float value;

	// Put value into float
	ErrorCheck(m_EventInstance->getParameterByName(name, &value));

	// Return float
	return value;
}

void AudioEvent::SetPosition(const glm::vec3& pos)
{
	// Update our attributes
	m_Attributes.position = VectorToFmod(pos);

	// Send update to fmod
	ErrorCheck(m_EventInstance->set3DAttributes(&m_Attributes));
}

glm::vec3 AudioEvent::GetPosition()
{
	return FmodToVector(m_Attributes.position);
}

////////////////////////////////////
//			 Audio Engine		  //
////////////////////////////////////

AudioEngine& AudioEngine::Instance()
{
	static AudioEngine instance;

	return instance;
}

void AudioEngine::Init()
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

void AudioEngine::Update()
{
	// Update FMOD system
	ErrorCheck(m_StudioSystem->update());

}

void AudioEngine::Shutdown()
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

	AudioEngine::ErrorCheck(m_StudioSystem->unloadAll());
	AudioEngine::ErrorCheck(m_StudioSystem->release());
}

void AudioEngine::LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
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

AudioListener& AudioEngine::GetListener()
{
	return m_Listener;
}

AudioEvent& AudioEngine::CreateEvent(const std::string& eventName, const std::string& GUID)
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
		AudioEvent* newAudioEvent = new AudioEvent(newFMODEvent);

		// Make sure multiple events with the same name aren't created
		if (m_EventMap.find(eventName) != m_EventMap.end())
		{
			__debugbreak();
		}

		// Add the audio event to our map
		m_EventMap[eventName] = newAudioEvent;

		return *newAudioEvent;
	}

}

AudioEvent& AudioEngine::GetEvent(const std::string& strEventName)
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

void AudioEngine::LoadBus(const std::string& strBusName, const std::string& GUID)
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
	AudioBus* newBus = new AudioBus(fmodBus);

	// Add the bus to our map
	m_BusMap[strBusName] = newBus;

	// Close your eyes for a bit, you deserve it
}

AudioBus& AudioEngine::GetBus(const std::string& strBusName)
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

void AudioEngine::SetGlobalParameter(const char* name, const float& value, const bool& ignoreSeekSpeed)
{
	ErrorCheck(m_StudioSystem->setParameterByName(name, value, ignoreSeekSpeed));
}

float AudioEngine::GetGlobalParameterValue(const char* name)
{
	// Make float for output
	float value;

	// Put value into float
	ErrorCheck(m_StudioSystem->getParameterByName(name, &value));

	// Return float
	return value;
}





