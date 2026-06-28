# 表单

# 旧版

```tsx
import { useState } from "react";

interface FormData {
  username: string;
  password: string;
}

export function Primary() {
    // 账号、密码状态
    const [formData, setFormData] = useState<FormData>({
        username: "",
        password: "",
    });

    // 账号、密码变化处理函数
    const nameChangeHandler = (event: React.ChangeEvent<HTMLInputElement>) => {
        const { name, value } = event.target;
        setFormData((prev) => ({
        ...prev,
        username: value,
        }));
    };

    const passwordChangeHandler = (event: React.ChangeEvent<HTMLInputElement>) => {
        const { name, value } = event.target;
        setFormData((prev) => ({
        ...prev,
        password: value,
        }));
    };

    // 表单提交处理函数
    const handleSubmit = (event: React.FormEvent<HTMLFormElement>) => {
        // 阻止默认提交行为，防止页面刷新
        event.preventDefault();

        console.log("Form submitted with data:", formData);
    };

    return (
        <form onSubmit={handleSubmit}>
            <label>
                <input type="text" name="username" onChange={nameChangeHandler} />
            </label>
            <label>
                <input type="text" name="password" onChange={passwordChangeHandler}/>
            </label>
            <button type="submit">Submit</button>
        </form>
    );
}
```

旧版代码实现表单提交太繁琐
- 需要自己维护状态、状态处理函数
- 需要手动阻止默认提交行为，防止页面刷新
- 提交失败后异常处理还要增加额外状态与处理逻辑

# formActin

在 `React 19` 中引入了 `formAction` 属性，大大简化表单提交过程。

```tsx
interface FormState {
  success: boolean;
}

export function MainForm() {
  // pre : 上一次 action 的处理结果
  // formData : 当前表单数据
  const handleAction = async (pre: FormState|null, formData: FormData) => {
    console.log([...formData.keys()]);
    console.log([...formData.values()]);
    console.log(pre);

    await new Promise(resolve => setTimeout(resolve, 1000));

    // action 的处理结果
    return {
      success: true
    } as FormState;
  };

  // result : 上一次 action 的处理结果
  // submitAction : 提交表单的函数
  // isPending : 表单是否正在提交中
  const [result, submitAction, isPending] = useActionState(handleAction,null);
  console.log(result);
  console.log(isPending);

  return (
    <form action={submitAction}>
      <label>
        <input type="text" name="username" />
      </label>
      <label>
        <input type="text" name="password"/>
      </label>
      <button type="submit"> {isPending? "Submiting...":  "Submit"}</button>
    </form>
  );
}
```

# useFormStatus

方便表单信息快速传递给子组件，避免手动传递状态

```tsx
export function Child() {
    // 获取 MainForm 状态
    const {pending, data, method} = useFormStatus();
    return <></>
}

export function MainForm() {
    const handleAction = async (pre: FormState|null, formData: FormData) => {
        console.log([...formData.keys()]);
        console.log([...formData.values()]);
        console.log(pre);

        await new Promise(resolve => setTimeout(resolve, 1000));

        // action 的处理结果
        return {
        success: true
        } as FormState;
    };

    const [result, submitAction, isPending] = useActionState(handleAction,null);

    return (
        <form action={submitAction}>
            <label>
                <input type="text" name="username" />
            </label>
            <label>
                <input type="text" name="password"/>
            </label>
            <button type="submit"> {isPending? "Submiting...":  "Submit"}</button>

            <Child />
        </form>
    );
}
```