using System;
using UnityEngine;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;

public class DataManager : MonoBehaviour {

    public static DataManager instance;

    private int balance = 0;
    public int coins;

    private int currentLvl = 0;
    public int awakeningPoints = 0;

    public int gainedXP;
    private int experience = 0;
    public int nextLevelUp = 500;

    private int[] characterData = new int[10];

    public bool victory;

    void Awake () {

        if (instance == null)
        {
            DontDestroyOnLoad(gameObject);
            instance = this;
            Load();
        }
        else if(instance != this)
        {
            Destroy(gameObject);
        }
	}

    public void Save()
    {
        BinaryFormatter bf = new BinaryFormatter();

        FileStream file = File.Create(Application.persistentDataPath + "/playerdata.dat");
        PlayerData pData = new PlayerData();

        pData.mBalance = balance;
        pData.mCurrentLvl = currentLvl;
        pData.mExperience = experience;
        pData.mNextLevelUp = nextLevelUp;
        pData.mAwakeningPoints = awakeningPoints;

        pData.CharacterData = characterData;
      
        bf.Serialize(file, pData);

        file.Close();

        /*
        if (File.Exists(Application.persistentDataPath + "/playerdata.dat"))
        {
            FileStream file = File.Open(Application.persistentDataPath + "/playerdata.dat", FileMode.Open);
            PlayerData pData = (PlayerData)bf.Deserialize(file);

            pData.mBalance = balance;
            pData.mCurrentLvl = currentLvl;
            pData.mExperience = experience;
            pData.mNextLevelUp = nextLevelUp;
            pData.mAwakeningPoints = awakeningPoints;

            bf.Serialize(file, pData);

            file.Close();
        }
        else
        {
        
        FileStream file = File.Create(Application.persistentDataPath + "/playerdata.dat");
            PlayerData pData = new PlayerData();

            pData.mBalance = balance;
            pData.mCurrentLvl = currentLvl;
            pData.mExperience = experience;
            pData.mNextLevelUp = nextLevelUp;
            pData.mAwakeningPoints = awakeningPoints;

            bf.Serialize(file, pData);

            file.Close();
        }
        */
    }

    public void Load()
    {
        if (File.Exists(Application.persistentDataPath + "/playerdata.dat"))
        {
            BinaryFormatter bf = new BinaryFormatter();

            FileStream file = File.Open(Application.persistentDataPath + "/playerdata.dat", FileMode.Open);
            PlayerData pData = (PlayerData)bf.Deserialize(file);

            balance = pData.mBalance;
            currentLvl = pData.mCurrentLvl;
            experience = pData.mExperience;
            nextLevelUp = pData.mNextLevelUp;
            awakeningPoints = pData.mAwakeningPoints;


            characterData = pData.CharacterData;

            file.Close();
        }
        else
        {
            Save();
        }
    }

    public void Deposit()
    {
        balance += coins;
        experience += gainedXP;

        coins = 0;
        gainedXP = 0;

        Save();
    }

    public int ReturnLevel(int ID)
    {
        return characterData[ID];
    }

    public bool TimeChamber(int ID, int cost)
    {
        if (balance >= cost) {
            balance = balance - cost;
            characterData[ID] += 1;
            Save();
            return true;
        }

        return false;
    }

    public void GainExperience(int amount)
    {
        gainedXP += amount;
        if (experience >= nextLevelUp)
        {
            LevelUp();
        }
    }

    public int GetXP()
    {
        return gainedXP;
    }

    void LevelUp()
    {
        currentLvl += 1;
        nextLevelUp += 500;
        awakeningPoints += 1;
    }

    public void Reward()
    {
        coins = coins * 2;
    }

    public int GetBalance()
    {
        return balance;
    }
}

//Seriable Tag: Allows me to write class data to a File
[Serializable]
class PlayerData
{
    public int mBalance;
    public int mCurrentLvl;
    public int mExperience;
    public int mNextLevelUp;
    public int mAwakeningPoints;

    public int[] CharacterData = new int[10];
}

