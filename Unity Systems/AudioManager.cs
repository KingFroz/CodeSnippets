using System.Collections;
using UnityEngine;
using UnityEngine.Audio;

public class AudioManager : MonoBehaviour
{
    public enum AudioChannel { Master, SFX, Music };

    public float masterVolumePercent = 1;
    public float sfxVolumePercent = 1;
    public float musicVolumePercent = 1;

    AudioSource sfx2DSource;
    AudioSource vocalSfxSource;

    AudioSource[] aSources;
    int activeAudioSourceIndex;

    AudioMixer masterMixer;
    AudioMixer sfxMixer;

    public static AudioManager instance;

    //private Transform audiolistener;
    private Transform playerTransform;

    SoundLibrary library;

    [HideInInspector]
    public MusicManager musicManager;

    void Awake()
    {
        if (instance != null)
            Destroy(gameObject);
        else
        {
            instance = this;
            DontDestroyOnLoad(gameObject);

            masterMixer = Resources.Load("Mixer/MasterMixer", typeof(AudioMixer)) as AudioMixer;
            sfxMixer = Resources.Load("Mixer/SfxMixer", typeof(AudioMixer)) as AudioMixer;

            library = GetComponent<SoundLibrary>();
            aSources = new AudioSource[2];
            for (int i = 0; i < 2; i++)
            {
                GameObject newAudioSource = new GameObject("Audio source " + (i + 1));
                aSources[i] = newAudioSource.AddComponent<AudioSource>();
                newAudioSource.transform.parent = transform;
                newAudioSource.GetComponent<AudioSource>().outputAudioMixerGroup = masterMixer.FindMatchingGroups("Music")[0];
            }

            GameObject newSfx2DSource = new GameObject("2D Sfx source ");
            sfx2DSource = newSfx2DSource.AddComponent<AudioSource>();
            newSfx2DSource.transform.parent = transform;
            newSfx2DSource.GetComponent<AudioSource>().outputAudioMixerGroup = masterMixer.FindMatchingGroups("Sfx")[0];

            GameObject newvocalSfxSource = new GameObject("Vocal Sfx source ");
            vocalSfxSource = newvocalSfxSource.AddComponent<AudioSource>();
            newvocalSfxSource.transform.parent = transform;
            newvocalSfxSource.GetComponent<AudioSource>().outputAudioMixerGroup = sfxMixer.FindMatchingGroups("Vocals")[0];

            //audiolistener = FindObjectOfType<AudioListener>().transform;

            if (FindObjectOfType<Player>() != null) {
                playerTransform = FindObjectOfType<Player>().transform;
            }

            masterVolumePercent = PlayerPrefs.GetFloat(Utility.masterVol, masterVolumePercent);
            sfxVolumePercent = PlayerPrefs.GetFloat(Utility.sfxVol, sfxVolumePercent);
            musicVolumePercent = PlayerPrefs.GetFloat(Utility.musicVol, musicVolumePercent);

            musicManager = GetComponent<MusicManager>();

            QualitySettings.vSyncCount = 0;
            Application.targetFrameRate = 60;
        }
    }

    public void SetPlayerTransform()
    {
        if (playerTransform != null)
            return;

        playerTransform = FindObjectOfType<Player>().transform;
    }

    public Vector3 GetPlayerTransform() {
        return playerTransform.position;
    }

    public void PlayMusic(AudioClip clip, bool loop, float fadeDuration = 1) {
        activeAudioSourceIndex = 1 - activeAudioSourceIndex;
        //aSources[activeAudioSourceIndex].Stop();
        aSources[activeAudioSourceIndex].clip = clip;
        aSources[activeAudioSourceIndex].Play();
        aSources[activeAudioSourceIndex].loop = loop;

        StartCoroutine(AnimateAudioCrossfade(fadeDuration));
    }

    public void SetVolume(float volumePercent, AudioChannel channel) {
        switch (channel)
        {
            case AudioChannel.Master:
                masterVolumePercent = volumePercent;
                break;
            case AudioChannel.SFX:
                sfxVolumePercent = volumePercent;
                break;
            case AudioChannel.Music:
                musicVolumePercent = volumePercent;
                break;
        }

        aSources[activeAudioSourceIndex].volume = musicVolumePercent * masterVolumePercent;
        //aSources[1].volume = musicVolumePercent * masterVolumePercent;

        PlayerPrefs.SetFloat(Utility.masterVol, masterVolumePercent);
        PlayerPrefs.SetFloat(Utility.sfxVol, sfxVolumePercent);
        PlayerPrefs.SetFloat(Utility.musicVol, musicVolumePercent);
        PlayerPrefs.Save();
    }

    public void PlaySound(AudioClip clip, Vector3 pos) {
        if (clip != null)
            AudioSource.PlayClipAtPoint(clip, pos, sfxVolumePercent * masterVolumePercent);
    }

    public void PlaySound(byte _id, Vector3 pos) {
        PlaySound(library.GetClipFromName(_id), pos);
    }

    public void PlaySound2D(byte _id) {
        sfx2DSource.PlayOneShot(library.GetClipFromName(_id), sfxVolumePercent * masterVolumePercent);
    }

    public void PlaySound2D(int _id)
    {
        sfx2DSource.PlayOneShot(library.GetClipFromName((byte)_id), sfxVolumePercent * masterVolumePercent);
    }

    public void PlayVocal2D(byte _id) {
        vocalSfxSource.PlayOneShot(library.GetClipFromName(_id), sfxVolumePercent * masterVolumePercent);
    }

    public bool isPlaying() { return aSources[activeAudioSourceIndex].isPlaying; }

    IEnumerator AnimateAudioCrossfade(float duration) {
        float percent = 0;

        while (percent < 1) {
            percent += Time.deltaTime * 1 / duration;
            //Fade Sound In
            aSources[activeAudioSourceIndex].volume = Mathf.Lerp(0, musicVolumePercent * masterVolumePercent, percent);
            //Fade Sound out
            aSources[1 - activeAudioSourceIndex].volume = Mathf.Lerp(musicVolumePercent * masterVolumePercent, 0, percent);

            yield return null;
        }
    }
}