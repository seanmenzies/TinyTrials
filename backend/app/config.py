from pydantic_settings import BaseSettings, SettingsConfigDict

class Settings(BaseSettings):
    app_env: str = "local"
    app_host: str = "0.0.0.0"
    app_port: int = 8000

    database_url: str

    jwt_secret: str
    jwt_alg: str = "HS256"
    jwt_exp_minutes: int = 60 * 24 * 30

    model_config = SettingsConfigDict(env_file=".env", case_sensitive=False)

settings = Settings()