
using System;
using UnityEngine;

public abstract class Entity : PoolObject, IDamagable
{
    public float baseHealth;
    public float baseEndurance;
    public float baseMovementSpeed;
    public float baseShield;

    public float refillRate;

    public int wallet;

    protected float m_Health;
    protected float mEndurance;
    protected float mShield;
    protected float movementSpeed;

    protected float refillDelay;

    protected bool isPlayer;
    protected bool isDead;

    private bool invulnerable;
    private float invulnerabilityTimer;

    public event Action OnDeath;

    public enum GuardState { Open, OnGuard, Parry, Shield }
    protected GuardState guardState;

    public Animator animator;

    protected CapsuleCollider hurtBox;

    float staminaRegenTimer;

    //Allow Start function to be overridden
    protected virtual void Start() {
        m_Health = baseHealth;
        mEndurance = baseEndurance;
        mShield = baseShield;

        guardState = GuardState.Open;
        isDead = false;
        hurtBox = null;
        refillDelay = 0;

        staminaRegenTimer = Time.time;

        if (gameObject.tag == Utility.playerStr)
            isPlayer = true;
        else
            isPlayer = false;

        OnDeath += AIManager.instance.gameManager.OnEnemyDeath;
    }

    protected virtual void Start(CharacterLevel data)
    {
        baseHealth = data.healthPoints;
        baseEndurance = data.endurancePoints;
        //baseMovementSpeed = data.movementSpeed;

        m_Health = baseHealth;
        mEndurance = baseEndurance;
        mShield = baseShield;
        movementSpeed = baseMovementSpeed;

        guardState = GuardState.Open;
        isDead = false;
        hurtBox = null;
        refillDelay = 0;

        staminaRegenTimer = Time.time;

        if (gameObject.tag == Utility.playerStr)
            isPlayer = true;
        else
            isPlayer = false;

        invulnerabilityTimer = Time.time + 0.5f;

        OnDeath += AIManager.instance.gameManager.OnEnemyDeath;
    }


    protected virtual void FixedUpdate()
    {
        if (!AIManager.instance.GetActive())
            invulnerable = true;
        else if (invulnerabilityTimer > Time.time)
            invulnerable = true;
        else
            invulnerable = false;

        if (animator.GetCurrentAnimatorStateInfo(1).IsName(Utility.def))
            guardState = GuardState.OnGuard;
        else if (animator.GetCurrentAnimatorStateInfo(1).IsName(Utility.par))
            guardState = GuardState.Parry;
        else
            guardState = GuardState.Open;

        if (Time.time > staminaRegenTimer)
        {
            StaminaRegen();
            staminaRegenTimer = Time.time + 0.025f;
        }
    }

    public void SetInvulnerabilityTimer(float _time)
    {
        PoolManager.instance.ReuseObject(ParticleLibrary.instance.GetParticleFromID(15), transform.position);
        invulnerabilityTimer = Time.time + _time;
    }

    public bool GetInvulnerability()
    {
        return invulnerable;
    }

    public bool isAlive()
    {
        if (isDead)
            return false;

        return true;
    }

    public bool GetPlayer()
    {
        return isPlayer;
    }

    public void DmgPopUp(float _dmg)
    {
        if (_dmg == 1)
            PoolManager.instance.ReuseObject(PopUpLibrary.instance.GetPopUpFromID(1), transform.position + Vector3.up, Quaternion.identity);
        else if (_dmg == 2)
            PoolManager.instance.ReuseObject(PopUpLibrary.instance.GetPopUpFromID(2), transform.position + Vector3.up, Quaternion.identity);
        else if (_dmg == 3)
            PoolManager.instance.ReuseObject(PopUpLibrary.instance.GetPopUpFromID(3), transform.position + Vector3.up, Quaternion.identity);
    }


    public virtual void TakeHit(float _damage, Vector3 _hitPoint, Vector3 _hitDirection) {
        if (invulnerable)
            return;

        switch (guardState)
        {
            case GuardState.Open:
                PoolManager.instance.ReuseObject(ParticleLibrary.instance.GetParticleFromID(Utility.hitParticle), transform.position + Vector3.up * 0.5f, Quaternion.identity, Vector3.one);
                CameraShake.instance.StartShake();
                AudioManager.instance.PlaySound2D(Utility.grunt);

                if (!animator.GetCurrentAnimatorStateInfo(1).IsName(Utility.tkDmg) && !animator.GetCurrentAnimatorStateInfo(1).IsName(Utility.stunned))
                    animator.SetTrigger(Utility.hit);

                m_Health = _damage < 0 ? 0 : m_Health - _damage;
                DmgPopUp(_damage);
                //PoolManager.instance.ReuseObject(PopUpLibrary.instance.GetPopUpFromID(1), transform.position + Vector3.up, Quaternion.identity);

                if (isPlayer)
                    TMPro.Example.Feedback.instance.SlowMo();

                break;
            case GuardState.OnGuard:
                AudioManager.instance.PlayVocal2D(Utility.shdHit);
                CameraShake.instance.StartShake();

                float manaCost = _damage * 50;

                //IF Entity doesn't have enough stamina to BLOCK
                if (GetMana() < 0 || GetMana() < manaCost)
                {
                    PoolManager.instance.ReuseObject(ParticleLibrary.instance.GetParticleFromID(Utility.grdbrkParticle), transform.position + Vector3.up * 0.5f, Quaternion.identity, Vector3.one);

                    if (isPlayer)
                        TMPro.Example.Feedback.instance.SlowMo();

                    if (!animator.GetCurrentAnimatorStateInfo(1).IsName(Utility.tkDmg) && !animator.GetCurrentAnimatorStateInfo(1).IsName(Utility.stunned))
                        animator.SetTrigger(Utility.hit);

                    TakeMana(manaCost);
                    m_Health = _damage < 0 ? 0 : m_Health - _damage;
                }
                else
                {
                    PoolManager.instance.ReuseObject(ParticleLibrary.instance.GetParticleFromID(Utility.blkParticle), transform.position + Vector3.up * 0.5f, Quaternion.identity, Vector3.one);
                    TakeMana(manaCost);
                }
                break;
            case GuardState.Parry:
                BoostMana(25);

                if (isPlayer)
                {
                    TMPro.Example.Feedback.instance.SlowMo();
                    TMPro.Example.Feedback.instance.SendMSG(Utility.par);
                }
                
                AudioManager.instance.PlaySound2D(Utility.parSfx);
                PoolManager.instance.ReuseObject(ParticleLibrary.instance.GetParticleFromID(Utility.parParticle), transform.position + Vector3.up * 0.5f, Quaternion.identity, Vector3.one);
                break;
            case GuardState.Shield:
                mShield = _damage < 0 ? 0 : mShield - _damage;

                AudioManager.instance.PlayVocal2D(Utility.shdHit);
                CameraShake.instance.StartShake();

                PoolManager.instance.ReuseObject(ParticleLibrary.instance.GetParticleFromID(Utility.blkParticle), transform.position + Vector3.up * 0.5f, Quaternion.identity, Vector3.one);
                break;
        }
        
        if (m_Health <= 0 && !isDead) {
            DeathEvent();
        }
    }

    public virtual void TakeDamage(float dmg, bool _unblockable, Entity _entity) {
        if (invulnerable)
            return;

        switch (guardState)
        {
            case GuardState.Open:
                PoolManager.instance.ReuseObject(ParticleLibrary.instance.GetParticleFromID(Utility.hitParticle), transform.position + Vector3.up * 0.5f, Quaternion.identity, Vector3.one);
                AudioManager.instance.PlayVocal2D(Utility.slash);
                CameraShake.instance.StartShake();

                if (!animator.GetCurrentAnimatorStateInfo(1).IsName(Utility.tkDmg) && !animator.GetCurrentAnimatorStateInfo(1).IsName(Utility.stunned))
                    animator.SetTrigger(Utility.hit);

                m_Health -= dmg;
                DmgPopUp(dmg);
                //PoolManager.instance.ReuseObject(PopUpLibrary.instance.GetPopUpFromID(1), transform.position + Vector3.up, Quaternion.identity);

                if (isPlayer)
                    TMPro.Example.Feedback.instance.SlowMo();

                break;
            case GuardState.OnGuard:
                AudioManager.instance.PlayVocal2D(Utility.shdHit);
                CameraShake.instance.StartShake();

                //IF attack is Unblockable, just take Health, ELSE check stamina
                if (_unblockable)
                {
                    PoolManager.instance.ReuseObject(ParticleLibrary.instance.GetParticleFromID(Utility.hitParticle), transform.position + Vector3.up * 0.5f, Quaternion.identity, Vector3.one);

                    if (!animator.GetCurrentAnimatorStateInfo(1).IsName(Utility.tkDmg) && !animator.GetCurrentAnimatorStateInfo(1).IsName(Utility.stunned))
                        animator.SetTrigger(Utility.hit);

                    m_Health -= dmg;
                }
                else
                {
                    float manaCost = dmg * 15;
                    //IF Entity doesn't have enough stamina to BLOCK
                    if (GetMana() < 0 || GetMana() < manaCost)
                    {
                        PoolManager.instance.ReuseObject(ParticleLibrary.instance.GetParticleFromID(Utility.grdbrkParticle), transform.position + Vector3.up * 0.5f, Quaternion.identity, Vector3.one);

                        if (isPlayer)
                            TMPro.Example.Feedback.instance.SlowMo();

                        if (!animator.GetCurrentAnimatorStateInfo(1).IsName(Utility.tkDmg) && !animator.GetCurrentAnimatorStateInfo(1).IsName(Utility.stunned))
                            animator.SetTrigger(Utility.hit);

                        TakeMana(manaCost);
                        m_Health -= dmg;
                    }
                    else
                    {
                        PoolManager.instance.ReuseObject(ParticleLibrary.instance.GetParticleFromID(Utility.blkParticle), transform.position + Vector3.up * 0.5f, Quaternion.identity, Vector3.one);
                        TakeMana(manaCost);
                    }
                }

                break;
            case GuardState.Parry:
                if (_entity != null)
                {
                    _entity.animator.SetTrigger(Utility.stunned);
                    BoostMana(25);

                    if (isPlayer)
                    {
                        TMPro.Example.Feedback.instance.SlowMo();
                        TMPro.Example.Feedback.instance.SendMSG(Utility.par);
                    }
                    
                    AudioManager.instance.PlaySound2D(Utility.parSfx);
                    PoolManager.instance.ReuseObject(ParticleLibrary.instance.GetParticleFromID(Utility.parParticle), transform.position + Vector3.up * 0.5f, Quaternion.identity, Vector3.one);
                }
                else
                {
                    AudioManager.instance.PlayVocal2D(Utility.shdHit);
                    CameraShake.instance.StartShake();

                    float manaCost = dmg * 25;

                    //IF Entity doesn't have enough stamina to BLOCK
                    if (GetMana() < 0 || GetMana() < manaCost)
                    {
                        PoolManager.instance.ReuseObject(ParticleLibrary.instance.GetParticleFromID(Utility.hitParticle), transform.position + Vector3.up * 0.5f, Quaternion.identity, Vector3.one);

                        if (!animator.GetCurrentAnimatorStateInfo(1).IsName(Utility.tkDmg) || !animator.GetCurrentAnimatorStateInfo(1).IsName(Utility.stunned))
                            animator.SetTrigger(Utility.hit);

                        m_Health -= dmg;
                    }
                    else
                    {
                        PoolManager.instance.ReuseObject(ParticleLibrary.instance.GetParticleFromID(Utility.blkParticle), transform.position + Vector3.up * 0.5f, Quaternion.identity, Vector3.one);
                        TakeMana(manaCost);
                    }
                }

                break;
            case GuardState.Shield:
                mShield = dmg < 0 ? 0 : mShield - dmg;

                AudioManager.instance.PlayVocal2D(Utility.shdHit);
                CameraShake.instance.StartShake();

                PoolManager.instance.ReuseObject(ParticleLibrary.instance.GetParticleFromID(Utility.blkParticle), transform.position + Vector3.up * 0.5f, Quaternion.identity, Vector3.one);
                break;
        }
        

        if (m_Health <= 0 && !isDead) {
            PoolManager.instance.ReuseObject(ParticleLibrary.instance.GetParticleFromID(Utility.dthParticle), transform.position + Vector3.up * 0.5f, Quaternion.identity, Vector3.one);
            DeathEvent();
        }
    }

    void IncrDelay() {
        refillDelay = 15;
    }

    public bool TakeMana(float amount) {
        mEndurance = mEndurance - amount < 0 ? 0 : mEndurance - amount;
        IncrDelay();

        return mEndurance > 0 ? true : false;
    }

    public float GetHealth() { return m_Health; }
    public float GetMana() { return mEndurance; }

    public void BoostMana(float amount) {
        mEndurance = mEndurance + amount > baseEndurance ? mEndurance = baseEndurance : mEndurance + amount;
    }

    protected void DeathEvent() {
        isDead = true;
        hurtBox.enabled = false;

        if (!isPlayer)
        {
            PoolManager.instance.ReuseObject(PopUpLibrary.instance.GetPopUpFromID(0), AIManager.instance.GetTarget().position + Vector3.up, Quaternion.identity);
            DataManager.instance.GainExperience(10);
        }

        for (int i = 0; i < wallet; i++) {
            PoolManager.instance.ReuseObject(ParticleLibrary.instance.GetParticleFromID(Utility.coinParticle), transform.position, transform.rotation);
        }

        AudioManager.instance.PlayVocal2D(Utility.splat);
        AudioManager.instance.PlayVocal2D(Utility.deathGrunts);
        AIManager.instance.IncrTokens();

        animator.SetBool(Utility.isD, true);

        gameObject.layer = Utility.inactiveLayer;

        OnDeath();
    }

    public void CallDestroy()
    {
        Destroy();
    }

    public void StaminaRegen()
    {
        if (!isDead && mEndurance < baseEndurance)
        {
            //What is this saying?
            if (refillDelay > 0)
                refillDelay -= 1;
            else
                mEndurance += refillRate;

            if (mEndurance > baseEndurance)
                mEndurance = baseEndurance;
        }
    }
}
