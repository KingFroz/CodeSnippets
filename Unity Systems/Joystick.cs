
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class Joystick : MonoBehaviour, IDragHandler, IPointerUpHandler, IPointerDownHandler {

    private Vector3 outputVector;
    private Vector3 unnormalizedOutput;

    private Vector3[] fourCorners = new Vector3[4];
    private Vector2 bgImageStartPos;

    private Image knobImage;
    private Image bgImage;

    private bool fixedPosition = true;

    // Use this for initialization
    void Start () {
        if (GetComponent<Image>() != null && transform.GetChild(0).GetComponent<Image>() != null)
        {
            bgImage = GetComponent<Image>();
            knobImage = transform.GetChild(0).GetComponent<Image>();
            bgImage.rectTransform.GetWorldCorners(fourCorners); 

            bgImageStartPos = fourCorners[3];
            bgImage.rectTransform.pivot = new Vector2(1f, 0f);

            bgImage.rectTransform.anchorMin = new Vector2(0, 0);
            bgImage.rectTransform.anchorMax = new Vector2(0, 0);
            bgImage.rectTransform.position = bgImageStartPos;
        }
	}

    public Vector3 GetDirection() { return new Vector3(outputVector.x, outputVector.y, 0); }

    public bool GetFixed() { return fixedPosition; }
    public void SetFixed(bool _setter) { fixedPosition = _setter; }

    public Image GetBackgroundImage() { return bgImage; }
    public Image GetKnobImage() { return knobImage; } 

    public virtual void OnDrag(PointerEventData eventData)
    {
        Vector2 outPoint = Vector2.zero;

        if (RectTransformUtility.ScreenPointToLocalPointInRectangle(bgImage.rectTransform, eventData.position, eventData.pressEventCamera, out outPoint))
        {
            outPoint.x = (outPoint.x / bgImage.rectTransform.sizeDelta.x);
            outPoint.y = (outPoint.y / bgImage.rectTransform.sizeDelta.y);

            outputVector = new Vector3(outPoint.x * 2 + 1, outPoint.y * 2 - 1, 0);
            unnormalizedOutput = outputVector;

            outputVector = (outputVector.magnitude > 1.0f) ? outputVector.normalized : outputVector;

            knobImage.rectTransform.anchoredPosition =
            new Vector3(outputVector.x * (bgImage.rectTransform.sizeDelta.x * 0.25f),
                        outputVector.y * (bgImage.rectTransform.sizeDelta.y * 0.25f));

            if (fixedPosition == false)
            {
                if (unnormalizedOutput.magnitude > outputVector.magnitude)
                {
                    var currentPosition = bgImage.rectTransform.position;

                    currentPosition.x += eventData.delta.x;
                    currentPosition.y += eventData.delta.y;

                    currentPosition.x = Mathf.Clamp(currentPosition.x, 0 + bgImage.rectTransform.sizeDelta.x, Screen.width * 0.5f);
                    currentPosition.y = Mathf.Clamp(currentPosition.y, 0, Screen.height - bgImage.rectTransform.sizeDelta.y);

                    bgImage.rectTransform.position = currentPosition;
                }
            }
        }
    }

    public virtual void OnPointerDown(PointerEventData eventData)
    {
        OnDrag(eventData);
        //fixedPosition = true;
    }

    public virtual void OnPointerUp(PointerEventData eventData)
    {
        outputVector = Vector3.zero;
        knobImage.rectTransform.anchoredPosition = Vector3.zero;
        //fixedPosition = false;
    }

    void OnDisable()
    {
        outputVector = Vector3.zero;
        knobImage.rectTransform.anchoredPosition = Vector3.zero;
        //fixedPosition = false;
    }
}
