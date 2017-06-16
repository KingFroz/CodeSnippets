//using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PartManager : MonoBehaviour {
    [Header("Custom Parts")]
    public GameObject[] m_BarrelParts;
    public GameObject[] m_StockParts;

    public List<GameObject> activeBarrelList = new List<GameObject>();
    public List<GameObject> activeStockList = new List<GameObject>();

    public int m_BarrelIndex;
    public int m_StockIndex;
    public int m_C02Index;

    public void Toggle()
    {
        AssignParts("Barrel", m_BarrelIndex);
        AssignParts("Stock", m_StockIndex);
    }

    public void SetIndex(string name, int setter)
    {
        if (name  == "Barrel") {
            m_BarrelIndex = setter;
        } else if (name  == "Stock") {
            m_StockIndex = setter;
        }

        Toggle();
        //weaponControl.Reset();
    }

    public int GetBarrelIndex() { return m_BarrelIndex; }
    public int GetStockIndex() { return m_StockIndex; }

    //Function: Sets active weapon part
    private void AssignParts(string name, int index)
    {
        if (name == "Barrel")
        {
            for (int i = 0; i < activeBarrelList.Count; i++)
            {
                if (i == index) {
                    activeBarrelList[i].gameObject.SetActive(true);
                } else {
                    activeBarrelList[i].gameObject.SetActive(false);
                }
            }
        } else if (name == "Stock") {
            for (int i = 0; i < activeStockList.Count; i++)
            {
                if (i == index) {
                    activeStockList[i].gameObject.SetActive(true);
                } else {
                    activeStockList[i].gameObject.SetActive(false);
                }
            }
        }
    }

    public int GetNumBarrelParts() { return activeBarrelList.Count; }
    public int GetNumStockParts() { return activeStockList.Count; }

    public void AddStock(int _index) {
        if (!activeStockList.Contains(m_StockParts[_index]))
        {
            activeStockList.Add(m_StockParts[_index]);
        }
    }

    public void AddBarrel(int _index) {
        if (!activeBarrelList.Contains(m_BarrelParts[_index]))
        {
            activeBarrelList.Add(m_BarrelParts[_index]);
        }
    }
}
