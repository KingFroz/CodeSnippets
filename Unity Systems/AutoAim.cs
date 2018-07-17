
using UnityEngine;
using System.Collections.Generic;

public class AutoAim : MonoBehaviour {

    Vector3 vectorToTarget;

    float baseLength;

    Quaternion identity;

    Transform nearestTarget;

    List<Transform> avaibleTargets = new List<Transform>(5);

    void Start() {
        identity = Quaternion.identity;
    }

    void FixedUpdate()
    {
        if (avaibleTargets.Count > 0) {
            SelectTarget();
        } else if (avaibleTargets.Count < 1 || transform.localRotation.x != 0 || transform.localRotation.y != 0 || transform.localRotation.z != 0) {
            transform.localRotation = identity;
        }
    }

    void SelectTarget()
    {
        baseLength = -1f;
        int length = avaibleTargets.Count;

        for (int i = 0; i < length; i++)
        {
            if (avaibleTargets[i].gameObject.layer == Utility.inactiveLayer || avaibleTargets[i].gameObject.activeSelf == false) {
                avaibleTargets.Remove(avaibleTargets[i]);
                length--;
                continue;
            }

            vectorToTarget = avaibleTargets[i].transform.position - transform.position;
            vectorToTarget.Normalize();

            float lengthOfAngle = Vector3.Dot(transform.forward, vectorToTarget);
            if (lengthOfAngle > baseLength)
            {
                nearestTarget = avaibleTargets[i].transform;
                baseLength = lengthOfAngle;
            }

            transform.LookAt(nearestTarget);
        }
    }

    //Entity in range of another Entity
    void OnTriggerEnter(Collider col)
    {
        if (col.CompareTag(Utility.enemyStr))
            avaibleTargets.Add(col.transform);
    }

    void OnTriggerExit(Collider col)
    {
        avaibleTargets.Remove(col.transform);
        if (avaibleTargets.Count <= 0)
        {
            transform.localRotation = identity;
        }
    }
}
