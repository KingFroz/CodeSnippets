using UnityEngine;
using System.Collections;
using UnityEngine.UI;

public class ResourceManager : MonoBehaviour
{
    public int m_MaxResource = 100;
    private int m_Resource;
    private float m_RefillRate;
    [Space]
    public Slider slider;

    // Use this for initialization
    void Start()
    {
        m_Resource = m_MaxResource = 100;
        m_RefillRate = 0.25f;

        StartCoroutine("ManaCharge");
    }

    public bool TakeResouces(int _minus)
    {
        if (_minus > m_Resource)
        {
            return false;
        }
        m_Resource = (int)Mathf.Max(0, m_Resource - _minus);
        slider.value = m_Resource;
        return true;
    }

    private void Regain()
    {
        m_Resource = (int)Mathf.Min(m_MaxResource, m_Resource + 1);
        slider.value = m_Resource;
    }

    private IEnumerator ManaCharge()
    {
        while (true)
        {
            Regain();
            yield return new WaitForSeconds(m_RefillRate);
        }
    }
}
