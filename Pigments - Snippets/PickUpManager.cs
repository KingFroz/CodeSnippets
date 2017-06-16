using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PickUpManager : MonoBehaviour
{
    public List<GameObject> m_PickableObjects;

    private AudioSource aSource;
    public AudioClip aClip;

    private float delay = 0.5f;

    MeshRenderer Mesh;

    private void Start()
    {
        Mesh = GetComponentInChildren<MeshRenderer>();
        aSource = GetComponent<AudioSource>();
        if (m_PickableObjects.Count > 0)
            Instantiate(m_PickableObjects[Mathf.Min(m_PickableObjects.Count - 1, (int)Random.Range(0.0f, m_PickableObjects.Count))], GetComponentInChildren<VRTK.Examples.AutoRotation>().transform).transform.localPosition = Vector3.zero;
        
    }
    void OnTriggerEnter(Collider other)
    {
        WeaponPickUp _PickUp = GetComponentInChildren<WeaponPickUp>();
        if (_PickUp != null)
        {
            //PartManager _PartManager = other.GetComponentInChildren<PartManager>();
            PartManager _PartManager = other.GetComponentInParent<PartManager>();
            if (_PartManager != null)
            {
                switch (_PickUp.m_Type)
                {
                    case WeaponPickUp.PickUpType.Barrel:
                        _PartManager.AddBarrel(_PickUp.m_BarrelAttributes.m_Index);
                        Mesh.enabled = false;
                        aSource.PlayOneShot(aClip);
                        break;
                    case WeaponPickUp.PickUpType.Stock:
                        _PartManager.AddStock(_PickUp.m_StockAttributes.m_Index);
                        Mesh.enabled = false;
                        aSource.PlayOneShot(aClip);
                        break;
                    default:
                        break;
                }

                StartCoroutine(Vanish());
            }
            
        }
    }

    private IEnumerator Vanish()
    {
        while (true)
        {
            yield return new WaitForSeconds(delay);
            Destroy(gameObject);
        }
    }
}
