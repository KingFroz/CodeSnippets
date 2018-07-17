
using UnityEngine;

public class Coin : PoolObject {

    Transform startPos;
    Transform endPos;

    float journeyTime = 7f;
    float speed = .5f;

    float startTime = 0;
    Vector3 center;
    Vector3 startRelCenter;
    Vector3 endRelCenter;

    float sleep;

    void Start()
    {
        endPos = AIManager.instance.GetPlayerEntity().transform;
    }

    void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag(Utility.playerStr))
        {
            DataManager.instance.coins += 5;
            //PlayerStats.intialCoin += 5;
            AudioManager.instance.PlaySound2D(Utility.coin);
            Destroy();
        }
    }

    public override void OnObjectReuse()
    {
        transform.rotation = Quaternion.Euler(0, Random.Range(0, 360), 0);
        startTime = Time.time;
        startPos = transform;
        sleep = Time.time + 2f;
    }

    void Update()
    {
        GetCenter(Vector3.up);
        if (Time.time > sleep)
        {
            float fracComplete = (Time.time - startTime) / journeyTime * speed;
            transform.position = Vector3.Slerp(startRelCenter, endRelCenter, fracComplete * speed);
            transform.position += center;
        }
    }


    void GetCenter(Vector3 dir)
    {
        //Calculate center
        center = (startPos.position + endPos.position) * .5f;
        center -= dir;
        startRelCenter = startPos.position - center;
        endRelCenter = endPos.position - center;
    }
}
